// Copyright 2017 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <assert.h>
#include <stdio.h>

#include <hid-parser/item.h>
#include <hid-parser/parser.h>
#include <hid-parser/usages.h>
#include <hid-parser/report.h>
#include <hid-parser/units.h>

#include <unistd.h>
#include <unittest/unittest.h>

// See hid-report-data.cpp for the definitions of the test data.
extern "C" const uint8_t boot_mouse_r_desc[50];
extern "C" const uint8_t hp_mouse_r_desc[46];
extern "C" const uint8_t trinket_r_desc[173];
extern "C" const uint8_t ps3_ds_r_desc[148];
extern "C" const uint8_t acer12_touch_r_desc[660];
extern "C" const uint8_t eve_tablet_r_desc[28];
extern "C" const uint8_t asus_touch_desc[945];
extern "C" const uint8_t eve_touchpad_v2_r_desc[560];

// See hid-utest-data.cpp for the definitions of the test data
extern "C" const uint8_t push_pop_test[62];

namespace {
struct Stats {
    int input_count;
    int collection[2];
};

size_t ItemizeHIDReportDesc(const uint8_t* rpt_desc, size_t desc_len, Stats* stats) {
    const uint8_t* buf = rpt_desc;
    size_t len = desc_len;
    while (len > 0) {
        size_t actual = 0;
        auto item = hid::Item::ReadNext(buf, len, &actual);
        if ((actual > len) || (actual == 0))
            break;

        if (item.tag() == hid::Item::Tag::kEndCollection)
            stats->collection[1]++;
        else if (item.tag() == hid::Item::Tag::kCollection)
            stats->collection[0]++;

        if (item.type() == hid::Item::Type::kMain && item.tag() == hid::Item::Tag::kInput)
            stats->input_count++;

        len -= actual;
        buf += actual;
    }

    return (desc_len - len);
}

}  // namespace.

static bool itemize_acer12_rpt1() {
    BEGIN_TEST;

    Stats stats = {};
    auto len = sizeof(acer12_touch_r_desc);
    auto consumed = ItemizeHIDReportDesc(acer12_touch_r_desc, len, &stats);

    ASSERT_EQ(consumed, len);
    ASSERT_EQ(stats.input_count, 45);
    ASSERT_EQ(stats.collection[0], 13);
    ASSERT_EQ(stats.collection[1], 13);

    END_TEST;
}

static bool itemize_eve_tablet_rpt() {
    BEGIN_TEST;

    Stats stats = {};
    auto len = sizeof(eve_tablet_r_desc);
    auto consumed = ItemizeHIDReportDesc(eve_tablet_r_desc, len, &stats);

    ASSERT_EQ(consumed, len);
    ASSERT_EQ(stats.input_count, 2);
    ASSERT_EQ(stats.collection[0], 1);
    ASSERT_EQ(stats.collection[1], 1);

    END_TEST;
}


static bool parse_boot_mouse() {
    BEGIN_TEST;

    hid::DeviceDescriptor* dev = nullptr;
    auto res = hid::ParseReportDescriptor(
        boot_mouse_r_desc, sizeof(boot_mouse_r_desc), &dev);

    ASSERT_EQ(res, hid::ParseResult::kParseOk);

    // A single report with id zero, this means no report id.
    ASSERT_EQ(dev->rep_count, 1u);
    EXPECT_EQ(dev->report[0].report_id, 0);

    // The only report has 6 fields and is 32 bits long.
    EXPECT_EQ(dev->report[0].input_count, 6);
    EXPECT_EQ(dev->report[0].input_byte_sz, 3);
    const auto fields = dev->report[0].input_fields;


    // All fields are input type with report id = 0.
    for (uint8_t ix = 0; ix != dev->report[0].input_count; ++ix) {
        EXPECT_EQ(fields[ix].report_id, 0);
        EXPECT_EQ(fields[ix].type, hid::kInput);
    }

    // First 3 fields are the buttons, with usages 1, 2, 3, in the button page.
    auto expected_flags = hid::kData | hid::kAbsolute | hid::kScalar;

    for (uint8_t ix = 0; ix != 3; ++ix) {
        EXPECT_EQ(fields[ix].attr.usage.page, hid::usage::Page::kButton);
        EXPECT_EQ(fields[ix].attr.usage.usage, uint32_t(ix + 1));
        EXPECT_EQ(fields[ix].attr.bit_sz, 1);
        EXPECT_EQ(fields[ix].attr.offset, ix);
        EXPECT_EQ(fields[ix].attr.logc_mm.min, 0);
        EXPECT_EQ(fields[ix].attr.logc_mm.max, 1);
        EXPECT_EQ(expected_flags & fields[ix].flags, expected_flags);
    }

    // Next field is 5 bits constant. Aka padding.
    EXPECT_EQ(fields[3].attr.bit_sz, 5);
    EXPECT_EQ(fields[3].attr.offset, 3);
    EXPECT_EQ(hid::kConstant & fields[3].flags, hid::kConstant);

    // Next comes 'X' field, 8 bits data, relative.
    expected_flags = hid::kData | hid::kRelative | hid::kScalar;

    EXPECT_EQ(fields[4].attr.usage.page, hid::usage::Page::kGenericDesktop);
    EXPECT_EQ(fields[4].attr.usage.usage, hid::usage::GenericDesktop::kX);
    EXPECT_EQ(fields[4].attr.bit_sz, 8);
    EXPECT_EQ(fields[4].attr.offset, 8);
    EXPECT_EQ(fields[4].attr.logc_mm.min, -127);
    EXPECT_EQ(fields[4].attr.logc_mm.max, 127);
    EXPECT_EQ(fields[4].attr.phys_mm.min, 0);
    EXPECT_EQ(fields[4].attr.phys_mm.max, 0);
    EXPECT_EQ(expected_flags & fields[4].flags, expected_flags);

    // Last comes 'Y' field, same as 'X'.
    EXPECT_EQ(fields[5].attr.usage.page, hid::usage::Page::kGenericDesktop);
    EXPECT_EQ(fields[5].attr.usage.usage, hid::usage::GenericDesktop::kY);
    EXPECT_EQ(fields[5].attr.bit_sz, 8);
    EXPECT_EQ(fields[5].attr.offset, 16);
    EXPECT_EQ(fields[5].attr.logc_mm.min, -127);
    EXPECT_EQ(fields[5].attr.logc_mm.max, 127);
    EXPECT_EQ(fields[5].attr.phys_mm.min, 0);
    EXPECT_EQ(fields[5].attr.phys_mm.max, 0);
    EXPECT_EQ(expected_flags & fields[4].flags, expected_flags);

    // Now test the collections.
    // Inner collection is physical GeneticDesktop|Pointer.
    auto collection = fields[0].col;
    ASSERT_TRUE(collection != nullptr);
    EXPECT_EQ(collection->type, hid::CollectionType::kPhysical);
    EXPECT_EQ(collection->usage.page, hid::usage::Page::kGenericDesktop);
    EXPECT_EQ(collection->usage.usage, hid::usage::GenericDesktop::kPointer);

    // Outer collection is the application.
    collection = collection->parent;
    ASSERT_TRUE(collection != nullptr);
    EXPECT_EQ(collection->type, hid::CollectionType::kApplication);
    EXPECT_EQ(collection->usage.page, hid::usage::Page::kGenericDesktop);
    EXPECT_EQ(collection->usage.usage, hid::usage::GenericDesktop::kMouse);

    // No parent collection.
    EXPECT_TRUE(collection->parent == nullptr);

    // Test the helpers.
    size_t count_first_input = 0;
    auto first_input = hid::GetFirstInputField(dev, &count_first_input);
    EXPECT_EQ(first_input, dev->report[0].input_fields);
    EXPECT_EQ(6u, count_first_input);

    auto app_col = hid::GetAppCollection(first_input);
    EXPECT_EQ(app_col, collection);


    hid::FreeDeviceDescriptor(dev);
    END_TEST;
}

static bool parse_hp_mouse() {
    BEGIN_TEST;

    hid::DeviceDescriptor* dev = nullptr;
    auto res = hid::ParseReportDescriptor(
        hp_mouse_r_desc, sizeof(hp_mouse_r_desc), &dev);

    ASSERT_EQ(res, hid::ParseResult::kParseOk);

    // A single report with id zero, this means no report id.
    ASSERT_EQ(dev->rep_count, 1u);
    EXPECT_EQ(dev->report[0].report_id, 0);

    // The only report has 11 fields.
    EXPECT_EQ(dev->report[0].input_count, 11);
    const auto fields = dev->report[0].input_fields;

    // All fields are input type with report id = 0.
    for (uint8_t ix = 0; ix != dev->report[0].input_count; ++ix) {
        EXPECT_EQ(fields[ix].report_id, 0);
        EXPECT_EQ(fields[ix].type, hid::kInput);
    }

    // First 8 fields are the buttons, with usages 1, 2, 3, 3 .. 3 in the button page.
    auto expected_flags = hid::kData | hid::kAbsolute | hid::kScalar;

    for (uint8_t ix = 0; ix != 8; ++ix) {
        auto usage = (ix < 3) ? uint32_t(ix + 1) : uint32_t(3);
        EXPECT_EQ(fields[ix].attr.usage.page, hid::usage::Page::kButton);
        EXPECT_EQ(fields[ix].attr.usage.usage, usage);
        EXPECT_EQ(fields[ix].attr.bit_sz, 1);
        EXPECT_EQ(fields[ix].attr.logc_mm.min, 0);
        EXPECT_EQ(fields[ix].attr.logc_mm.max, 1);
        EXPECT_EQ(expected_flags & fields[ix].flags, expected_flags);
    }

    // Next comes 'X' field, 8 bits data, relative.
    expected_flags = hid::kData | hid::kRelative | hid::kScalar;

    EXPECT_EQ(fields[8].attr.usage.page, hid::usage::Page::kGenericDesktop);
    EXPECT_EQ(fields[8].attr.usage.usage, hid::usage::GenericDesktop::kX);
    EXPECT_EQ(fields[8].attr.bit_sz, 8);
    EXPECT_EQ(fields[8].attr.logc_mm.min, -127);
    EXPECT_EQ(fields[8].attr.logc_mm.max, 127);
    EXPECT_EQ(fields[8].attr.phys_mm.min, 0);
    EXPECT_EQ(fields[8].attr.phys_mm.max, 0);
    EXPECT_EQ(expected_flags & fields[8].flags, expected_flags);

    // Next comes 'Y' field, same as 'X'.
    EXPECT_EQ(fields[9].attr.usage.page, hid::usage::Page::kGenericDesktop);
    EXPECT_EQ(fields[9].attr.usage.usage, hid::usage::GenericDesktop::kY);
    EXPECT_EQ(fields[9].attr.bit_sz, 8);
    EXPECT_EQ(fields[9].attr.logc_mm.min, -127);
    EXPECT_EQ(fields[9].attr.logc_mm.max, 127);
    EXPECT_EQ(fields[9].attr.phys_mm.min, 0);
    EXPECT_EQ(fields[9].attr.phys_mm.max, 0);
    EXPECT_EQ(expected_flags & fields[9].flags, expected_flags);

    // Last comes 'Wheel' field.
    EXPECT_EQ(fields[10].attr.usage.page, hid::usage::Page::kGenericDesktop);
    EXPECT_EQ(fields[10].attr.usage.usage, hid::usage::GenericDesktop::kWheel);
    EXPECT_EQ(fields[10].attr.bit_sz, 8);
    EXPECT_EQ(fields[10].attr.logc_mm.min, -127);
    EXPECT_EQ(fields[10].attr.logc_mm.max, 127);
    EXPECT_EQ(fields[10].attr.phys_mm.min, 0);
    EXPECT_EQ(fields[10].attr.phys_mm.max, 0);
    EXPECT_EQ(expected_flags & fields[10].flags, expected_flags);

    // Now test the collections.
    // Inner collection is physical GeneticDesktop|Pointer.
    auto collection = fields[0].col;
    ASSERT_TRUE(collection != nullptr);
    EXPECT_EQ(collection->type, hid::CollectionType::kPhysical);
    EXPECT_EQ(collection->usage.page, hid::usage::Page::kGenericDesktop);
    EXPECT_EQ(collection->usage.usage, hid::usage::GenericDesktop::kPointer);

    // Outer collection is the application.
    collection = collection->parent;
    ASSERT_TRUE(collection != nullptr);
    EXPECT_EQ(collection->type, hid::CollectionType::kApplication);
    EXPECT_EQ(collection->usage.page, hid::usage::Page::kGenericDesktop);
    EXPECT_EQ(collection->usage.usage, hid::usage::GenericDesktop::kMouse);

    // No parent collection.
    EXPECT_TRUE(collection->parent == nullptr);

    hid::FreeDeviceDescriptor(dev);
    END_TEST;
}

static bool parse_adaf_trinket() {
    BEGIN_TEST;

    hid::DeviceDescriptor* dev = nullptr;
    auto res = hid::ParseReportDescriptor(
        trinket_r_desc, sizeof(trinket_r_desc), &dev);

    ASSERT_EQ(res, hid::ParseResult::kParseOk);

    // Four different reports
    ASSERT_EQ(dev->rep_count, 4u);

    //////////////////////////////////////////////////////////////////////////////////
    // First report is the same as boot_mouse, except for the report id.
    EXPECT_EQ(dev->report[0].report_id, 1);
    ASSERT_EQ(dev->report[0].input_count, 6);
    EXPECT_EQ(dev->report[0].input_byte_sz, 4);
    const hid::ReportField* fields = dev->report[0].input_fields;

    // All fields are scalar input type with report id = 1.
    for (uint8_t ix = 0; ix != dev->report[0].input_count; ++ix) {
        EXPECT_EQ(fields[ix].report_id, 1);
        EXPECT_EQ(fields[ix].type, hid::kInput);
        EXPECT_EQ(hid::kScalar & fields[ix].flags, hid::kScalar);
    }

    // First 3 fields are the buttons, with usages 1, 2, 3, in the button page.
    auto expected_flags = hid::kData | hid::kAbsolute;

    for (uint8_t ix = 0; ix != 3; ++ix) {
        EXPECT_EQ(fields[ix].attr.usage.page, hid::usage::Page::kButton);
        EXPECT_EQ(fields[ix].attr.usage.usage, uint32_t(ix + 1));
        EXPECT_EQ(fields[ix].attr.bit_sz, 1);
        EXPECT_EQ(fields[ix].attr.offset, 8u + ix);
        EXPECT_EQ(fields[ix].attr.logc_mm.min, 0);
        EXPECT_EQ(fields[ix].attr.logc_mm.max, 1);
        EXPECT_EQ(expected_flags & fields[ix].flags, expected_flags);
    }

    // Next field is 5 bits constant. Aka padding.
    EXPECT_EQ(fields[3].attr.bit_sz, 5);
    EXPECT_EQ(hid::kConstant & fields[3].flags, hid::kConstant);

    // Next comes 'X' field, 8 bits data, relative.
    expected_flags = hid::kData | hid::kRelative;

    EXPECT_EQ(fields[4].attr.usage.page, hid::usage::Page::kGenericDesktop);
    EXPECT_EQ(fields[4].attr.usage.usage, hid::usage::GenericDesktop::kX);
    EXPECT_EQ(fields[4].attr.bit_sz, 8);
    EXPECT_EQ(fields[4].attr.offset, 16);
    EXPECT_EQ(fields[4].attr.logc_mm.min, -127);
    EXPECT_EQ(fields[4].attr.logc_mm.max, 127);
    EXPECT_EQ(expected_flags & fields[4].flags, expected_flags);

    // Last comes 'Y' field, same as 'X'.
    EXPECT_EQ(fields[5].attr.usage.page, hid::usage::Page::kGenericDesktop);
    EXPECT_EQ(fields[5].attr.usage.usage, hid::usage::GenericDesktop::kY);
    EXPECT_EQ(fields[5].attr.bit_sz, 8);
    EXPECT_EQ(fields[5].attr.offset, 24);
    EXPECT_EQ(fields[5].attr.logc_mm.min, -127);
    EXPECT_EQ(fields[5].attr.logc_mm.max, 127);
    EXPECT_EQ(expected_flags & fields[4].flags, expected_flags);

    // Now test the collections.
    // Inner collection is physical GeneticDesktop|Pointer.
    auto collection = fields[0].col;
    ASSERT_TRUE(collection != nullptr);
    EXPECT_EQ(collection->type, hid::CollectionType::kPhysical);
    EXPECT_EQ(collection->usage.page, hid::usage::Page::kGenericDesktop);
    EXPECT_EQ(collection->usage.usage, hid::usage::GenericDesktop::kPointer);

    // Outer collection is the application.
    collection = collection->parent;
    ASSERT_TRUE(collection != nullptr);
    EXPECT_EQ(collection->type, hid::CollectionType::kApplication);
    EXPECT_EQ(collection->usage.page, hid::usage::Page::kGenericDesktop);
    EXPECT_EQ(collection->usage.usage, hid::usage::GenericDesktop::kMouse);

    // No parent collection.
    EXPECT_TRUE(collection->parent == nullptr);

    //////////////////////////////////////////////////////////////////////////////////
    // Second  report is a keyboard with 20 fields and is 72 bits long.
    EXPECT_EQ(dev->report[1].report_id, 2);
    ASSERT_EQ(dev->report[1].input_count, 14);
    EXPECT_EQ(dev->report[1].input_byte_sz, 8);
    ASSERT_EQ(dev->report[1].output_count, 6);
    EXPECT_EQ(dev->report[1].output_byte_sz, 2);

    const hid::ReportField* output_fields = dev->report[1].output_fields;
    fields = dev->report[1].input_fields;

    // First 8 are input bits with usages 0xe0 to 0xe7 on the keyboard page.
    expected_flags = hid::kData | hid::kAbsolute | hid::kScalar;

    for (uint8_t ix = 0; ix != 8; ++ix) {
        EXPECT_EQ(fields[ix].type, hid::kInput);
        EXPECT_EQ(fields[ix].attr.usage.page, hid::usage::Page::kKeyboardKeypad);
        EXPECT_EQ(fields[ix].attr.usage.usage, uint32_t(ix + 0xe0));
        EXPECT_EQ(fields[ix].attr.bit_sz, 1);
        EXPECT_EQ(fields[ix].attr.offset, 8u + ix);
        EXPECT_EQ(fields[ix].attr.logc_mm.min, 0);
        EXPECT_EQ(fields[ix].attr.logc_mm.max, 1);
        EXPECT_EQ(expected_flags & fields[ix].flags, expected_flags);
    }

    // Next field is 8 bits padding (input).
    EXPECT_EQ(fields[8].attr.bit_sz, 8);
    EXPECT_EQ(fields[8].attr.offset, 16);
    EXPECT_EQ(fields[8].type, hid::kInput);
    EXPECT_EQ(hid::kConstant & fields[8].flags, hid::kConstant);

    // Next 5 fields are byte-sized key input array.
    expected_flags = hid::kData | hid::kAbsolute | hid::kArray;

    for (uint8_t ix = 9; ix != 14; ++ix) {
        EXPECT_EQ(fields[ix].type, hid::kInput);
        EXPECT_EQ(fields[ix].attr.usage.page, hid::usage::Page::kKeyboardKeypad);
        EXPECT_EQ(fields[ix].attr.bit_sz, 8);
        EXPECT_EQ(fields[ix].attr.offset, 24u + 8u * (ix - 9u));
        EXPECT_EQ(fields[ix].attr.usage.usage, 0);
        EXPECT_EQ(fields[ix].attr.logc_mm.min, 0);
        EXPECT_EQ(fields[ix].attr.logc_mm.max, 164);
        EXPECT_EQ(expected_flags & fields[ix].flags, expected_flags);
    }

    // Test the output fields (LED bits output, with usages NumLock(1) to Kana(5)).
    auto led_usage = static_cast<uint16_t>(hid::usage::LEDs::kNumLock);
    expected_flags = hid::kData | hid::kAbsolute | hid::kScalar;

    for (uint8_t ix = 0; ix != 5; ++ix) {
        EXPECT_EQ(output_fields[ix].type, hid::kOutput);
        EXPECT_EQ(output_fields[ix].attr.usage.page, hid::usage::Page::kLEDs);
        EXPECT_EQ(output_fields[ix].attr.bit_sz, 1);
        EXPECT_EQ(output_fields[ix].attr.offset, 8u + ix);
        EXPECT_EQ(output_fields[ix].attr.usage.usage, led_usage++);
        EXPECT_EQ(expected_flags & output_fields[ix].flags, expected_flags);
    }

    // Next field is 3 bits padding (output).
    EXPECT_EQ(output_fields[5].attr.bit_sz, 3);
    EXPECT_EQ(output_fields[5].attr.offset, 13u);
    EXPECT_EQ(output_fields[5].type, hid::kOutput);
    EXPECT_EQ(hid::kConstant & output_fields[5].flags, hid::kConstant);


    // All fields belong to the same collection
    collection = fields[0].col;

    for (uint8_t ix = 1; ix != 20; ++ix) {
        EXPECT_TRUE(fields[ix].col == collection);
    }

    ASSERT_TRUE(collection != nullptr);
    EXPECT_EQ(collection->type, hid::CollectionType::kApplication);
    EXPECT_EQ(collection->usage.page, hid::usage::Page::kGenericDesktop);
    EXPECT_EQ(collection->usage.usage, hid::usage::GenericDesktop::kKeyboard);
    // No parent collection.
    EXPECT_TRUE(collection->parent == nullptr);

    //////////////////////////////////////////////////////////////////////////////////
    // Third report, single 16 bit input array field (consumer control).  24 bits long.
    EXPECT_EQ(dev->report[2].report_id, 3);
    ASSERT_EQ(dev->report[2].input_count, 1);
    EXPECT_EQ(dev->report[2].input_byte_sz, 3);

    fields = dev->report[2].input_fields;

    expected_flags = hid::kData | hid::kAbsolute | hid::kArray;

    EXPECT_EQ(fields[0].type, hid::kInput);
    EXPECT_EQ(fields[0].attr.usage.page, hid::usage::Page::kConsumer);
    EXPECT_EQ(fields[0].attr.usage.usage, 0);
    EXPECT_EQ(fields[0].attr.logc_mm.min, 0);
    EXPECT_EQ(fields[0].attr.logc_mm.max, 572);
    EXPECT_EQ(fields[0].attr.bit_sz, 16);
    EXPECT_EQ(fields[0].attr.offset, 8);
    EXPECT_EQ(expected_flags & fields[0].flags, expected_flags);

    collection = fields[0].col;
    ASSERT_TRUE(collection != nullptr);
    EXPECT_EQ(collection->type, hid::CollectionType::kApplication);
    EXPECT_EQ(collection->usage.page, hid::usage::Page::kConsumer);
    EXPECT_EQ(collection->usage.usage, hid::usage::Consumer::kConsumerControl);
    // No parent collection.
    EXPECT_TRUE(collection->parent == nullptr);

    //////////////////////////////////////////////////////////////////////////////////
    // Fourth report is a 2 bit input (system control: sleep, wake-up, power-down)
    // 16 bits in total.

    EXPECT_EQ(dev->report[3].report_id, 4);
    ASSERT_EQ(dev->report[3].input_count, 2);
    ASSERT_EQ(dev->report[3].input_byte_sz, 2);

    fields = dev->report[3].input_fields;

    // First field is a 2 bit input array.
    expected_flags = hid::kData | hid::kAbsolute | hid::kArray;

    EXPECT_EQ(fields[0].type, hid::kInput);
    EXPECT_EQ(fields[0].attr.usage.page, hid::usage::Page::kGenericDesktop);
    // TODO(cpu): The |usage.usage| as parsed is incorrect. In this particular
    // case as the array input 1,2,3 should map to 0x82, 0x81, 0x83 which is not currently
    // supported in the model.
    EXPECT_EQ(fields[0].attr.usage.usage, hid::usage::GenericDesktop::kSystemSleep);
    EXPECT_EQ(fields[0].attr.logc_mm.min, 1);
    EXPECT_EQ(fields[0].attr.logc_mm.max, 3);
    EXPECT_EQ(fields[0].attr.bit_sz, 2);
    EXPECT_EQ(fields[0].attr.offset, 8);
    EXPECT_EQ(expected_flags & fields[0].flags, expected_flags);

    // Last field is 6 bits padding (output).
    EXPECT_EQ(fields[1].attr.bit_sz, 6);
    EXPECT_EQ(fields[1].attr.offset, 10);
    EXPECT_EQ(fields[1].type, hid::kInput);
    EXPECT_EQ(hid::kConstant & fields[1].flags, hid::kConstant);

    collection = fields[0].col;
    ASSERT_TRUE(collection != nullptr);
    EXPECT_EQ(collection->type, hid::CollectionType::kApplication);
    EXPECT_EQ(collection->usage.page, hid::usage::Page::kGenericDesktop);
    EXPECT_EQ(collection->usage.usage, hid::usage::GenericDesktop::kSystemControl);
    // No parent collection.
    EXPECT_TRUE(collection->parent == nullptr);

    hid::FreeDeviceDescriptor(dev);
    END_TEST;
}

static bool parse_ps3_controller() {
    BEGIN_TEST;

    hid::DeviceDescriptor* dev = nullptr;
    auto res = hid::ParseReportDescriptor(
        ps3_ds_r_desc, sizeof(ps3_ds_r_desc), &dev);

    ASSERT_EQ(res, hid::ParseResult::kParseOk);
    // Four different reports
    ASSERT_EQ(dev->rep_count, 4u);

    //////////////////////////////////////////////////////////////////////////////////
    // First report has 172 fields!!  1160 bits long!!!
    EXPECT_EQ(dev->report[0].report_id, 1);

    ASSERT_EQ(dev->report[0].input_count, 76);
    ASSERT_EQ(dev->report[0].input_byte_sz, 49);

    ASSERT_EQ(dev->report[0].output_count, 48);
    ASSERT_EQ(dev->report[0].output_byte_sz, 49);

    ASSERT_EQ(dev->report[0].feature_count, 48);
    ASSERT_EQ(dev->report[0].feature_byte_sz, 49);
    const hid::ReportField* fields = dev->report[0].input_fields;
    const hid::ReportField* output_fields = dev->report[0].output_fields;
    const hid::ReportField* feature_fields = dev->report[0].feature_fields;

    // First field is 8 bits, constant GenericDesktop page, but no usage described.
    // being it is a version number?
    auto expected_flags = hid::kConstant | hid::kAbsolute | hid::kScalar;

    EXPECT_EQ(fields[0].type, hid::kInput);
    EXPECT_EQ(fields[0].attr.usage.page, hid::usage::Page::kGenericDesktop);
    EXPECT_EQ(fields[0].attr.usage.usage, 0);
    EXPECT_EQ(fields[0].attr.logc_mm.min, 0);
    EXPECT_EQ(fields[0].attr.logc_mm.max, 255);
    EXPECT_EQ(fields[0].attr.bit_sz, 8);
    EXPECT_EQ(fields[0].attr.offset, 8);
    EXPECT_EQ(expected_flags & fields[0].flags, expected_flags);

    // Next 19 fields are one-bit input representing the buttons.
    expected_flags = hid::kData | hid::kAbsolute | hid::kScalar;

    for (uint8_t ix = 1; ix != 20; ++ix) {
        EXPECT_EQ(fields[ix].type, hid::kInput);
        EXPECT_EQ(fields[ix].attr.usage.page, hid::usage::Page::kButton);
        EXPECT_EQ(fields[ix].attr.usage.usage, ix);
        EXPECT_EQ(fields[ix].attr.bit_sz, 1);
        EXPECT_EQ(fields[ix].attr.offset, 16u + (ix - 1u));
        EXPECT_EQ(fields[ix].attr.logc_mm.min, 0);
        EXPECT_EQ(fields[ix].attr.logc_mm.max, 1);
        EXPECT_EQ(fields[ix].attr.phys_mm.min, 0);
        EXPECT_EQ(fields[ix].attr.phys_mm.max, 1);
        EXPECT_EQ(expected_flags & fields[ix].flags, expected_flags);
    }

    // The next 13 fields are 13 bits of constant, vendor-defined. Probably padding.
    for (uint8_t ix = 20; ix != 33; ++ix) {
        EXPECT_EQ(fields[ix].type, hid::kInput);
        EXPECT_EQ(fields[ix].attr.usage.page, hid::usage::Page::kVendorDefinedStart);
        EXPECT_EQ(fields[ix].attr.usage.usage, 0);
        EXPECT_EQ(fields[ix].attr.bit_sz, 1);
        EXPECT_EQ(fields[ix].attr.offset, 35u + (ix - 20u));
        EXPECT_EQ(hid::kConstant & fields[ix].flags, hid::kConstant);
    }

    expected_flags = hid::kData | hid::kAbsolute | hid::kScalar;

    // Next four 8-bit input fields are X,Y, Z and Rz.
    for (uint8_t ix = 33; ix != 37; ++ix) {
        EXPECT_EQ(fields[ix].type, hid::kInput);
        EXPECT_EQ(fields[ix].attr.usage.page, hid::usage::Page::kGenericDesktop);
        EXPECT_EQ(fields[ix].attr.bit_sz, 8);
        EXPECT_EQ(fields[ix].attr.offset, 48u + 8u * (ix - 33u));
        EXPECT_EQ(fields[ix].attr.logc_mm.min, 0);
        EXPECT_EQ(fields[ix].attr.logc_mm.max, 255);
        EXPECT_EQ(fields[ix].attr.phys_mm.min, 0);
        EXPECT_EQ(fields[ix].attr.phys_mm.max, 255);
        EXPECT_EQ(expected_flags & fields[ix].flags, expected_flags);
    }

    EXPECT_EQ(fields[33].attr.usage.usage, hid::usage::GenericDesktop::kX);
    EXPECT_EQ(fields[34].attr.usage.usage, hid::usage::GenericDesktop::kY);
    EXPECT_EQ(fields[35].attr.usage.usage, hid::usage::GenericDesktop::kZ);
    EXPECT_EQ(fields[36].attr.usage.usage, hid::usage::GenericDesktop::kRz);

    // Next 39 fields are input, 8-bit pointer scalar data.
    for (uint8_t ix = 37; ix != 76; ++ix) {
        EXPECT_EQ(fields[ix].type, hid::kInput);
        EXPECT_EQ(fields[ix].attr.usage.page, hid::usage::Page::kGenericDesktop);
        EXPECT_EQ(fields[ix].attr.usage.usage, hid::usage::GenericDesktop::kPointer);
        EXPECT_EQ(fields[ix].attr.bit_sz, 8);
        EXPECT_EQ(fields[ix].attr.offset, 80u + 8u * (ix - 37u));
        EXPECT_EQ(expected_flags & fields[ix].flags, expected_flags);
    }

    // Test the 48 8-bit scalar output pointer data.
    for (uint8_t ix = 0; ix != 48; ++ix) {
        EXPECT_EQ(output_fields[ix].type, hid::kOutput);
        EXPECT_EQ(output_fields[ix].attr.usage.page, hid::usage::Page::kGenericDesktop);
        EXPECT_EQ(output_fields[ix].attr.usage.usage, hid::usage::GenericDesktop::kPointer);
        EXPECT_EQ(output_fields[ix].attr.bit_sz, 8);
        EXPECT_EQ(output_fields[ix].attr.offset, 8u + 8u * ix);
        EXPECT_EQ(expected_flags & output_fields[ix].flags, expected_flags);
    }

    // Test the 48 8-bit scalar feature pointer data.
    for (uint8_t ix = 0; ix != 48; ++ix) {
        EXPECT_EQ(feature_fields[ix].type, hid::kFeature);
        EXPECT_EQ(feature_fields[ix].attr.usage.page, hid::usage::Page::kGenericDesktop);
        EXPECT_EQ(feature_fields[ix].attr.usage.usage, hid::usage::GenericDesktop::kPointer);
        EXPECT_EQ(feature_fields[ix].attr.bit_sz, 8);
        EXPECT_EQ(feature_fields[ix].attr.offset, 8u + 8u * (ix));
        EXPECT_EQ(expected_flags & feature_fields[ix].flags, expected_flags);
    }

    //////////////////////////////////////////////////////////////////////////////////
    // Second report has 48 fields. It is pretty much identical to last 48 fields
    // of the first report.  392 bits long.

    EXPECT_EQ(dev->report[1].report_id, 2);
    ASSERT_EQ(dev->report[1].feature_count, 48);
    ASSERT_EQ(dev->report[2].feature_byte_sz, 49);
    feature_fields = dev->report[1].feature_fields;

    expected_flags = hid::kData | hid::kAbsolute | hid::kScalar;

    for (uint8_t ix = 0; ix != 48; ++ix) {
        EXPECT_EQ(feature_fields[ix].type, hid::kFeature);
        EXPECT_EQ(feature_fields[ix].attr.usage.page, hid::usage::Page::kGenericDesktop);
        EXPECT_EQ(feature_fields[ix].attr.usage.usage, hid::usage::GenericDesktop::kPointer);
        EXPECT_EQ(feature_fields[ix].attr.bit_sz, 8);
        EXPECT_EQ(feature_fields[ix].attr.offset, 8u + 8u * ix);
        EXPECT_EQ(expected_flags & feature_fields[ix].flags, expected_flags);
    }

    //////////////////////////////////////////////////////////////////////////////////
    // Third report is same as the second one except for report id.

    EXPECT_EQ(dev->report[2].report_id, 0xee);
    ASSERT_EQ(dev->report[2].feature_count, 48);
    ASSERT_EQ(dev->report[2].feature_byte_sz, 49);
    feature_fields = dev->report[2].feature_fields;

    expected_flags = hid::kData | hid::kAbsolute | hid::kScalar;

    for (uint8_t ix = 0; ix != 48; ++ix) {
        EXPECT_EQ(feature_fields[ix].type, hid::kFeature);
        EXPECT_EQ(feature_fields[ix].attr.usage.page, hid::usage::Page::kGenericDesktop);
        EXPECT_EQ(feature_fields[ix].attr.usage.usage, hid::usage::GenericDesktop::kPointer);
        EXPECT_EQ(feature_fields[ix].attr.bit_sz, 8);
        EXPECT_EQ(feature_fields[ix].attr.offset, 8u + 8u * ix);
        EXPECT_EQ(expected_flags & feature_fields[ix].flags, expected_flags);
    }

    //////////////////////////////////////////////////////////////////////////////////
    // Fourth report is same as the second one except for report id.

    EXPECT_EQ(dev->report[3].report_id, 0xef);
    ASSERT_EQ(dev->report[2].feature_count, 48);
    ASSERT_EQ(dev->report[2].feature_byte_sz, 49);
    feature_fields = dev->report[3].feature_fields;

    expected_flags = hid::kData | hid::kAbsolute | hid::kScalar;

    for (uint8_t ix = 0; ix != 48; ++ix) {
        EXPECT_EQ(feature_fields[ix].type, hid::kFeature);
        EXPECT_EQ(feature_fields[ix].attr.usage.page, hid::usage::Page::kGenericDesktop);
        EXPECT_EQ(feature_fields[ix].attr.usage.usage, hid::usage::GenericDesktop::kPointer);
        EXPECT_EQ(feature_fields[ix].attr.bit_sz, 8);
        EXPECT_EQ(feature_fields[ix].attr.offset, 8u + 8u * ix);
        EXPECT_EQ(expected_flags & feature_fields[ix].flags, expected_flags);
    }

    // Collections test
    //
    // In the first report, The X,Y,Z, Rz fields are in a 3-level
    // deep collection physical -> logical -> app. Test that.
    auto collection = dev->report[0].input_fields[33].col;

    ASSERT_TRUE(collection != nullptr);
    EXPECT_EQ(collection->type, hid::CollectionType::kPhysical);
    EXPECT_EQ(collection->usage.page, hid::usage::Page::kGenericDesktop);
    EXPECT_EQ(collection->usage.usage, hid::usage::GenericDesktop::kPointer);

    collection = collection->parent;
    ASSERT_TRUE(collection != nullptr);
    EXPECT_EQ(collection->type, hid::CollectionType::kLogical);
    EXPECT_EQ(collection->usage.page, hid::usage::Page::kGenericDesktop);
    EXPECT_EQ(collection->usage.usage, 0);

    collection = collection->parent;
    ASSERT_TRUE(collection != nullptr);
    EXPECT_EQ(collection->type, hid::CollectionType::kApplication);
    EXPECT_EQ(collection->usage.page, hid::usage::Page::kGenericDesktop);
    EXPECT_EQ(collection->usage.usage, hid::usage::GenericDesktop::kJoystick);
    EXPECT_TRUE(collection->parent == nullptr);

    // The second report first field is in a logical -> app collection.
    collection = dev->report[1].input_fields[0].col;

    ASSERT_TRUE(collection != nullptr);
    EXPECT_EQ(collection->type, hid::CollectionType::kLogical);
    EXPECT_EQ(collection->usage.page, hid::usage::Page::kGenericDesktop);
    EXPECT_EQ(collection->usage.usage, 0);

    collection = collection->parent;
    ASSERT_TRUE(collection != nullptr);
    EXPECT_EQ(collection->type, hid::CollectionType::kApplication);
    EXPECT_EQ(collection->usage.page, hid::usage::Page::kGenericDesktop);
    EXPECT_EQ(collection->usage.usage, hid::usage::GenericDesktop::kJoystick);
    EXPECT_TRUE(collection->parent == nullptr);

    // The third report is the same as the second. This seems a trivial test
    // but previous parsers failed this one.
    collection = dev->report[2].input_fields[0].col;

    ASSERT_TRUE(collection != nullptr);
    EXPECT_EQ(collection->type, hid::CollectionType::kLogical);
    EXPECT_EQ(collection->usage.page, hid::usage::Page::kGenericDesktop);
    EXPECT_EQ(collection->usage.usage, 0);

    collection = collection->parent;
    ASSERT_TRUE(collection != nullptr);
    EXPECT_EQ(collection->type, hid::CollectionType::kApplication);
    EXPECT_EQ(collection->usage.page, hid::usage::Page::kGenericDesktop);
    EXPECT_EQ(collection->usage.usage, hid::usage::GenericDesktop::kJoystick);
    EXPECT_TRUE(collection->parent == nullptr);

    size_t ff_count = 0u;
    auto top_col = hid::GetAppCollection(hid::GetFirstInputField(dev, &ff_count));
    ASSERT_TRUE(top_col != nullptr);
    EXPECT_EQ(top_col, collection);
    EXPECT_EQ(ff_count, 172);

    hid::FreeDeviceDescriptor(dev);
    END_TEST;
}

static bool parse_acer12_touch() {
    BEGIN_TEST;

    hid::DeviceDescriptor* dd = nullptr;
    auto res = hid::ParseReportDescriptor(
        acer12_touch_r_desc, sizeof(acer12_touch_r_desc), &dd);

    EXPECT_EQ(res, hid::ParseResult::kParseOk);

    hid::FreeDeviceDescriptor(dd);
    END_TEST;
}

static bool parse_eve_tablet() {
    BEGIN_TEST;

    hid::DeviceDescriptor* dev = nullptr;
    auto res = hid::ParseReportDescriptor(
        eve_tablet_r_desc, sizeof(eve_tablet_r_desc), &dev);

    EXPECT_EQ(res, hid::ParseResult::kParseOk);

    // A single report, no id.
    ASSERT_EQ(dev->rep_count, 1u);
    EXPECT_EQ(dev->report[0].report_id, 0);

    // Report has two fields.  8 bits long
    ASSERT_EQ(dev->report[0].input_count, 2u);
    ASSERT_EQ(dev->report[0].input_byte_sz, 1u);

    const hid::ReportField* fields = dev->report[0].input_fields;

    // First field is 1 bit, (tablet / no-tablet)
    auto expected_flags = hid::kData | hid::kAbsolute | hid::kScalar;

    EXPECT_EQ(fields[0].type, hid::kInput);
    EXPECT_EQ(fields[0].attr.usage.page, hid::usage::Page::kGenericDesktop);
    EXPECT_EQ(fields[0].attr.usage.usage, 0xff000001);
    EXPECT_EQ(fields[0].attr.bit_sz, 1);
    EXPECT_EQ(fields[0].attr.offset, 0);
    EXPECT_EQ(expected_flags & fields[0].flags, expected_flags);

    // Second field is padding, 7 bits.
    expected_flags = hid::kConstant | hid::kAbsolute | hid::kScalar;

    EXPECT_EQ(fields[1].type, hid::kInput);
    EXPECT_EQ(fields[1].attr.usage.page, hid::usage::Page::kGenericDesktop);
    EXPECT_EQ(fields[1].attr.usage.usage, 0);
    EXPECT_EQ(fields[1].attr.bit_sz, 7);
    EXPECT_EQ(fields[1].attr.offset, 1);
    EXPECT_EQ(expected_flags & fields[1].flags, expected_flags);

    hid::FreeDeviceDescriptor(dev);
    END_TEST;
}

static bool parse_asus_touch() {
   BEGIN_TEST;
   hid::DeviceDescriptor* dev = nullptr;
   auto res = hid::ParseReportDescriptor(asus_touch_desc, sizeof(asus_touch_desc), &dev);
   ASSERT_EQ(res, hid::ParseResult::kParseOk);
   END_TEST;
}

// Test that the push and pop operations complete successfully.
// Pushing saves all of the GLOBAL items.
// Popping restores the previously saved GLOBAL items
static bool parse_push_pop() {
    BEGIN_TEST;
    hid::DeviceDescriptor* dev = nullptr;
    auto res = hid::ParseReportDescriptor(push_pop_test, sizeof(push_pop_test), &dev);
    ASSERT_EQ(res, hid::ParseResult::kParseOk);

    // A single report with id zero, this means no report id.
    ASSERT_EQ(dev->rep_count, 1u);
    EXPECT_EQ(dev->report[0].report_id, 0);

    // The only report has 12 fields.
    EXPECT_EQ(dev->report[0].input_count, 12);
    const auto fields = dev->report[0].input_fields;

    // All fields are input type with report id = 0.
    for (uint8_t ix = 0; ix != dev->report[0].input_count; ++ix) {
        EXPECT_EQ(fields[ix].report_id, 0);
        EXPECT_EQ(fields[ix].type, hid::kInput);
    }

    // First 3 fields are the buttons, with usages 1, 2, 3, in the button page.
    auto expected_flags = hid::kData | hid::kAbsolute | hid::kScalar;

    for (uint8_t ix = 0; ix != 3; ++ix) {
        EXPECT_EQ(fields[ix].attr.usage.page, hid::usage::Page::kButton);
        EXPECT_EQ(fields[ix].attr.usage.usage, uint32_t(ix + 1));
        EXPECT_EQ(fields[ix].attr.bit_sz, 1);
        EXPECT_EQ(fields[ix].attr.logc_mm.min, 0);
        EXPECT_EQ(fields[ix].attr.logc_mm.max, 1);
        EXPECT_EQ(expected_flags & fields[ix].flags, expected_flags);
    }

    // Next field is 5 bits constant. Aka padding.
    EXPECT_EQ(fields[3].attr.bit_sz, 5);
    EXPECT_EQ(hid::kConstant & fields[3].flags, hid::kConstant);

    // Next comes 'X' field, 8 bits data, relative.
    expected_flags = hid::kData | hid::kRelative | hid::kScalar;

    EXPECT_EQ(fields[4].attr.usage.page, hid::usage::Page::kGenericDesktop);
    EXPECT_EQ(fields[4].attr.usage.usage, hid::usage::GenericDesktop::kX);
    EXPECT_EQ(fields[4].attr.bit_sz, 8);
    EXPECT_EQ(fields[4].attr.logc_mm.min, -127);
    EXPECT_EQ(fields[4].attr.logc_mm.max, 127);
    EXPECT_EQ(fields[4].attr.phys_mm.min, 0);
    EXPECT_EQ(fields[4].attr.phys_mm.max, 0);
    EXPECT_EQ(expected_flags & fields[4].flags, expected_flags);

    // Next comes 'Y' field, same as 'X'.
    EXPECT_EQ(fields[5].attr.usage.page, hid::usage::Page::kGenericDesktop);
    EXPECT_EQ(fields[5].attr.usage.usage, hid::usage::GenericDesktop::kY);
    EXPECT_EQ(fields[5].attr.bit_sz, 8);
    EXPECT_EQ(fields[5].attr.logc_mm.min, -127);
    EXPECT_EQ(fields[5].attr.logc_mm.max, 127);
    EXPECT_EQ(fields[5].attr.phys_mm.min, 0);
    EXPECT_EQ(fields[5].attr.phys_mm.max, 0);
    EXPECT_EQ(expected_flags & fields[4].flags, expected_flags);

    // Next comes 'X' and 'Y' field again
    expected_flags = hid::kData | hid::kRelative | hid::kScalar;
    EXPECT_EQ(fields[6].attr.usage.page, hid::usage::Page::kGenericDesktop);
    EXPECT_EQ(fields[6].attr.usage.usage, 0);
    EXPECT_EQ(fields[6].attr.bit_sz, 8);
    EXPECT_EQ(fields[6].attr.logc_mm.min, -127);
    EXPECT_EQ(fields[6].attr.logc_mm.max, 127);
    EXPECT_EQ(fields[6].attr.phys_mm.min, 0);
    EXPECT_EQ(fields[6].attr.phys_mm.max, 0);
    EXPECT_EQ(expected_flags & fields[6].flags, expected_flags);
    EXPECT_EQ(fields[7].attr.usage.page, hid::usage::Page::kGenericDesktop);
    EXPECT_EQ(fields[7].attr.usage.usage, 0);
    EXPECT_EQ(fields[7].attr.bit_sz, 8);
    EXPECT_EQ(fields[7].attr.logc_mm.min, -127);
    EXPECT_EQ(fields[7].attr.logc_mm.max, 127);
    EXPECT_EQ(fields[7].attr.phys_mm.min, 0);
    EXPECT_EQ(fields[7].attr.phys_mm.max, 0);
    EXPECT_EQ(expected_flags & fields[7].flags, expected_flags);

    // Next is the popped padding field
    EXPECT_EQ(fields[8].attr.bit_sz, 5);
    EXPECT_EQ(hid::kConstant & fields[8].flags, hid::kConstant);

    // Last 3 fields are the popped buttons
    expected_flags = hid::kData | hid::kAbsolute | hid::kScalar;

    for (uint8_t ix = 9; ix != 12; ++ix) {
        EXPECT_EQ(fields[ix].attr.usage.page, hid::usage::Page::kButton);
        EXPECT_EQ(fields[ix].attr.usage.usage, 0);
        EXPECT_EQ(fields[ix].attr.bit_sz, 1);
        EXPECT_EQ(fields[ix].attr.logc_mm.min, 0);
        EXPECT_EQ(fields[ix].attr.logc_mm.max, 1);
        EXPECT_EQ(expected_flags & fields[ix].flags, expected_flags);
    }

    END_TEST;
}

static bool parse_eve_touchpad_v2() {
   BEGIN_TEST;
   hid::DeviceDescriptor* dev = nullptr;
   auto res = hid::ParseReportDescriptor(eve_touchpad_v2_r_desc,
                                         sizeof(eve_touchpad_v2_r_desc), &dev);
   ASSERT_EQ(res, hid::ParseResult::kParseOk);
   // Check that we have one main collection.
   EXPECT_EQ(dev->rep_count, 1);

   EXPECT_EQ(dev->report[0].report_id, 1);
   EXPECT_EQ(dev->report[0].input_count, 47);

   const hid::ReportField* fields = dev->report[0].input_fields;

   uint8_t ix = 0;

   // First report is a button.
   EXPECT_EQ(fields[ix].report_id, 1);
   EXPECT_EQ(fields[ix].type, hid::kInput);
   EXPECT_EQ(fields[ix].attr.usage.page, hid::usage::Page::kButton);
   EXPECT_EQ(fields[ix].attr.bit_sz, 1);
   ++ix;

   // Second report is a digitizer.
   EXPECT_EQ(fields[ix].report_id, 1);
   EXPECT_EQ(fields[ix].type, hid::kInput);
   EXPECT_EQ(fields[ix].attr.usage.page, hid::usage::Page::kDigitizer);
   EXPECT_EQ(fields[ix].attr.bit_sz, 7);
   ++ix;

   // Here are the finger collections. There are 10 items per finger.
   for (int finger = 0; finger != 5; ++finger) {
       char err_msg[50];
       sprintf(err_msg, "Failed on Finger %d\n", finger);

       EXPECT_EQ(fields[ix].type, hid::kInput);
       EXPECT_EQ(fields[ix].attr.usage.page, hid::usage::Page::kDigitizer, err_msg);
       EXPECT_EQ(fields[ix].attr.usage.usage, hid::usage::Digitizer::kTipSwitch, err_msg);
       EXPECT_EQ(fields[ix].attr.bit_sz, 1);
       ++ix;

       EXPECT_EQ(fields[ix].type, hid::kInput);
       EXPECT_EQ(fields[ix].attr.usage.page, hid::usage::Page::kDigitizer, err_msg);
       EXPECT_EQ(fields[ix].attr.usage.usage, hid::usage::Digitizer::kInRange, err_msg);
       EXPECT_EQ(fields[ix].attr.bit_sz, 7);
       ++ix;

       EXPECT_EQ(fields[ix].type, hid::kInput);
       EXPECT_EQ(fields[ix].attr.usage.page, hid::usage::Page::kDigitizer, err_msg);
       EXPECT_EQ(fields[ix].attr.usage.usage, 0x51, err_msg);
       EXPECT_EQ(fields[ix].attr.bit_sz, 16);
       ++ix;

       // The X coordinate.
       EXPECT_EQ(fields[ix].type, hid::kInput);
       EXPECT_EQ(fields[ix].attr.usage.page, hid::usage::Page::kGenericDesktop, err_msg);
       EXPECT_EQ(fields[ix].attr.usage.usage, hid::usage::GenericDesktop::kX, err_msg);
       EXPECT_EQ(fields[ix].attr.phys_mm.min, 0, err_msg);
       EXPECT_EQ(fields[ix].attr.phys_mm.max, 1030, err_msg);
       EXPECT_EQ(fields[ix].attr.logc_mm.max, 13184, err_msg);
       // TODO(dgilhooley) Define Unit types in ulib/hid-parser.
       EXPECT_EQ(fields[ix].attr.unit.type, 0x11, err_msg);
       EXPECT_EQ(fields[ix].attr.unit.exp, -2, err_msg);
       EXPECT_EQ(fields[ix].attr.bit_sz, 16, err_msg);
       ++ix;

       // The Y Coordinate (most fields are inherited from X).
       EXPECT_EQ(fields[ix].type, hid::kInput);
       EXPECT_EQ(fields[ix].attr.usage.page, hid::usage::Page::kGenericDesktop, err_msg);
       EXPECT_EQ(fields[ix].attr.usage.usage, hid::usage::GenericDesktop::kY, err_msg);
       EXPECT_EQ(fields[ix].attr.phys_mm.min, 0, err_msg);
       EXPECT_EQ(fields[ix].attr.phys_mm.max, 680, err_msg);
       EXPECT_EQ(fields[ix].attr.logc_mm.max, 8704, err_msg);
       // TODO(dgilhooley) Define Unit types in ulib/hid-parser
       EXPECT_EQ(fields[ix].attr.unit.type, 0x11, err_msg);
       EXPECT_EQ(fields[ix].attr.unit.exp, -2, err_msg);
       EXPECT_EQ(fields[ix].attr.bit_sz, 16, err_msg);
       ++ix;

       EXPECT_EQ(fields[ix].type, hid::kInput);
       EXPECT_EQ(fields[ix].attr.usage.page, hid::usage::Page::kDigitizer, err_msg);
       EXPECT_EQ(fields[ix].attr.usage.usage, 0x48, err_msg);
       EXPECT_EQ(fields[ix].attr.logc_mm.max, 13184, err_msg);
       EXPECT_EQ(fields[ix].attr.bit_sz, 16);
       ++ix;

       EXPECT_EQ(fields[ix].type, hid::kInput);
       EXPECT_EQ(fields[ix].attr.usage.page, hid::usage::Page::kDigitizer, err_msg);
       EXPECT_EQ(fields[ix].attr.usage.usage, 0x49, err_msg);
       EXPECT_EQ(fields[ix].attr.logc_mm.max, 8704, err_msg);
       EXPECT_EQ(fields[ix].attr.bit_sz, 16);
       ++ix;

       EXPECT_EQ(fields[ix].type, hid::kInput);
       EXPECT_EQ(fields[ix].attr.usage.page, hid::usage::Page::kDigitizer, err_msg);
       EXPECT_EQ(fields[ix].attr.usage.usage, hid::usage::Digitizer::kTipPressure, err_msg);
       EXPECT_EQ(fields[ix].attr.logc_mm.max, 255, err_msg);
       EXPECT_EQ(fields[ix].attr.bit_sz, 8);
       ++ix;

       EXPECT_EQ(fields[ix].type, hid::kInput);
       EXPECT_EQ(fields[ix].attr.usage.page, hid::usage::Page::kDigitizer, err_msg);
       EXPECT_EQ(fields[ix].attr.usage.usage, hid::usage::Digitizer::kAzimuth, err_msg);
       EXPECT_EQ(fields[ix].attr.logc_mm.max, 360, err_msg);
       EXPECT_EQ(fields[ix].attr.bit_sz, 16);
       ++ix;
   }

   // Make sure we checked each of the report fields.
   ASSERT_EQ(ix, dev->report[0].input_count);

   END_TEST;
}

static bool usage_helper() {
   BEGIN_TEST;
   auto usage = hid::USAGE(hid::usage::Page::kDigitizer, hid::usage::Digitizer::kContactID);
   EXPECT_EQ(usage.page, static_cast<uint16_t>(hid::usage::Page::kDigitizer));
   EXPECT_EQ(usage.usage, static_cast<uint32_t>(hid::usage::Digitizer::kContactID));
   END_TEST;
}

static bool minmax_operators() {
   BEGIN_TEST;
   EXPECT_TRUE((hid::MinMax{-1, 1} == hid::MinMax{-1, 1}));
   EXPECT_FALSE((hid::MinMax{0, 1} == hid::MinMax{-1, 1}));
   EXPECT_FALSE((hid::MinMax{-1, 1} == hid::MinMax{0, 1}));
   EXPECT_FALSE((hid::MinMax{-1, 2} == hid::MinMax{-1, 1}));
   EXPECT_FALSE((hid::MinMax{-1, 1} == hid::MinMax{-1, 2}));
   EXPECT_FALSE((hid::MinMax{0, 2} == hid::MinMax{-1, 1}));
   END_TEST;
}

static bool usage_operators() {
   BEGIN_TEST;
   EXPECT_TRUE(
      hid::USAGE(hid::usage::Page::kDigitizer, hid::usage::Digitizer::kContactID) ==
      hid::USAGE(hid::usage::Page::kDigitizer, hid::usage::Digitizer::kContactID)
   );
   EXPECT_FALSE(
      hid::USAGE(hid::usage::Page::kDigitizer, hid::usage::Digitizer::kTipSwitch) ==
      hid::USAGE(hid::usage::Page::kDigitizer, hid::usage::Digitizer::kContactID)
   );
   EXPECT_FALSE(
      hid::USAGE(hid::usage::Page::kGenericDesktop, hid::usage::GenericDesktop::kX) ==
      hid::USAGE(hid::usage::Page::kDigitizer, hid::usage::Digitizer::kContactID)
   );
   END_TEST;
}

static bool extract_tests() {
    BEGIN_TEST;
    uint8_t data[] = {0x0F, 0x0F, 0x0F, 0x0F, 0x0F};
    hid::Report report = {data, 5};
    hid::Attributes attr;
    bool ret;

    uint8_t int8;
    attr.offset = 0;
    attr.bit_sz = 8;
    ret = ExtractUint(report, attr, &int8);
    EXPECT_TRUE(ret);
    EXPECT_EQ(int8, 0x0F);

    attr.offset = 2;
    attr.bit_sz = 6;
    ret = ExtractUint(report, attr, &int8);
    EXPECT_TRUE(ret);
    EXPECT_EQ(int8, 0x03);

    attr.offset = 3;
    attr.bit_sz = 2;
    ret = ExtractUint(report, attr, &int8);
    EXPECT_TRUE(ret);
    EXPECT_EQ(int8, 0x01);

    // Test over a byte boundary
    attr.offset = 4;
    attr.bit_sz = 8;
    ret = ExtractUint(report, attr, &int8);
    EXPECT_TRUE(ret);
    EXPECT_EQ(int8, 0xF0);

    uint16_t int16;
    attr.offset = 0;
    attr.bit_sz = 16;
    ret = ExtractUint(report, attr, &int16);
    EXPECT_TRUE(ret);
    EXPECT_EQ(int16, 0x0F0F);

    attr.offset = 4;
    attr.bit_sz = 16;
    ret = ExtractUint(report, attr, &int16);
    EXPECT_TRUE(ret);
    EXPECT_EQ(int16, 0xF0F0);

    uint32_t int32;
    attr.offset = 0;
    attr.bit_sz = 32;
    ret = ExtractUint(report, attr, &int32);
    EXPECT_TRUE(ret);
    EXPECT_EQ(int32, 0x0F0F0F0F);

    attr.offset = 4;
    attr.bit_sz = 32;
    ret = ExtractUint(report, attr, &int32);
    EXPECT_TRUE(ret);
    EXPECT_EQ(int32, 0xF0F0F0F0);

    // Test that it fails if the attr is too large
    attr.offset = 0;
    attr.bit_sz = 9;
    ret = ExtractUint(report, attr, &int8);
    EXPECT_FALSE(ret);

    // Test that it fails if it goes past the end of the report
    attr.offset = 36;
    attr.bit_sz = 16;
    ret = ExtractUint(report, attr, &int16);
    EXPECT_FALSE(ret);

    END_TEST;

}

static bool unit_tests() {
    BEGIN_TEST;
    hid::Unit unit_in;
    hid::Unit unit_out;
    bool ret;
    double val_out;

    // Test the unit type setting/getting.
    {
        unit_in.type = 0;
        hid::unit::SetSystem(hid::unit::System::si_linear, unit_in);
        hid::unit::SetLengthExp(unit_in, 2);
        hid::unit::SetMassExp(unit_in, 3);
        hid::unit::SetTimeExp(unit_in, 7);
        hid::unit::SetTemperatureExp(unit_in, -1);
        hid::unit::SetCurrentExp(unit_in, -2);
        hid::unit::SetLuminousExp(unit_in, -8);

        auto sys = hid::unit::GetSystem(unit_in);
        EXPECT_EQ(sys, hid::unit::System::si_linear);

        int exp = hid::unit::GetLengthExp(unit_in);
        EXPECT_EQ(exp, 2);
        exp = hid::unit::GetMassExp(unit_in);
        EXPECT_EQ(exp, 3);
        exp = hid::unit::GetTimeExp(unit_in);
        EXPECT_EQ(exp, 7);
        exp = hid::unit::GetTemperatureExp(unit_in);
        EXPECT_EQ(exp, -1);
        exp = hid::unit::GetCurrentExp(unit_in);
        EXPECT_EQ(exp, -2);
        exp = hid::unit::GetLuminousExp(unit_in);
        EXPECT_EQ(exp, -8);
    }

    // Test same units convert to lower exponent.
    // 1 * 10^0 cm = 100 * 10^-1 cm
    unit_in.type = 0;
    hid::unit::SetSystem(hid::unit::System::si_linear, unit_in);
    hid::unit::SetLengthExp(unit_in, 1);
    unit_in.exp = 0;

    unit_out.type = 0;
    hid::unit::SetSystem(hid::unit::System::si_linear, unit_out);
    hid::unit::SetLengthExp(unit_out, 1);
    unit_out.exp = -2;

    ret = hid::unit::ConvertUnits(1, unit_in, val_out, unit_out);
    EXPECT_TRUE(ret);
    EXPECT_EQ(static_cast<int32_t>(val_out), 100);

    // Test same units convert to higher exponent.
    // 100 * 10^0 cm = 1 * 10^2 cm
    unit_in.type = 0;
    hid::unit::SetSystem(hid::unit::System::si_linear, unit_in);
    hid::unit::SetLengthExp(unit_in, 1);
    unit_in.exp = 0;

    unit_out.type = 0;
    hid::unit::SetSystem(hid::unit::System::si_linear, unit_out);
    hid::unit::SetLengthExp(unit_out, 1);
    unit_out.exp = 2;

    ret = hid::unit::ConvertUnits(100, unit_in, val_out, unit_out);
    EXPECT_TRUE(ret);
    EXPECT_EQ(static_cast<int32_t>(val_out), 1);

    // Distance Conversion Tests.

    // 100 * 10^1 inches == 25 * 10^2 cm
    unit_in.type = 0;
    hid::unit::SetSystem(hid::unit::System::eng_linear, unit_in);
    hid::unit::SetLengthExp(unit_in, 1);
    unit_in.exp = 1;

    unit_out.type = 0;
    hid::unit::SetSystem(hid::unit::System::si_linear, unit_out);
    hid::unit::SetLengthExp(unit_out, 1);
    unit_out.exp = 2;

    ret = hid::unit::ConvertUnits(100, unit_in, val_out, unit_out);
    EXPECT_TRUE(ret);
    EXPECT_EQ(static_cast<int32_t>(val_out), 25);

    // 1 * 10^2 cm == 39 * 10^0 in
    unit_in.type = 0;
    hid::unit::SetSystem(hid::unit::System::si_linear, unit_in);
    hid::unit::SetLengthExp(unit_in, 1);
    unit_in.exp = 2;

    unit_out.type = 0;
    hid::unit::SetSystem(hid::unit::System::eng_linear, unit_out);
    hid::unit::SetLengthExp(unit_out, 1);
    unit_out.exp = 0;

    ret = hid::unit::ConvertUnits(1, unit_in, val_out, unit_out);
    EXPECT_TRUE(ret);
    EXPECT_EQ(static_cast<int32_t>(val_out), 39);

    // 100 * 10^0 cm^3 == 6 * 10^0 in^3
    unit_in.type = 0;
    hid::unit::SetSystem(hid::unit::System::si_linear, unit_in);
    hid::unit::SetLengthExp(unit_in, 3);
    unit_in.exp = 0;

    unit_out.type = 0;
    hid::unit::SetSystem(hid::unit::System::eng_linear, unit_out);
    hid::unit::SetLengthExp(unit_out, 3);
    unit_out.exp = 0;

    ret = hid::unit::ConvertUnits(100, unit_in, val_out, unit_out);
    EXPECT_TRUE(ret);
    EXPECT_EQ(static_cast<int32_t>(val_out), 6);

    // 1 * 10^0 in^3 == 16 * 10^0 cm^3
    unit_in.type = 0;
    hid::unit::SetSystem(hid::unit::System::eng_linear, unit_in);
    hid::unit::SetLengthExp(unit_in, 3);
    unit_in.exp = 0;

    unit_out.type = 0;
    hid::unit::SetSystem(hid::unit::System::si_linear, unit_out);
    hid::unit::SetLengthExp(unit_out, 3);
    unit_out.exp = 0;

    ret = hid::unit::ConvertUnits(1, unit_in, val_out, unit_out);
    EXPECT_TRUE(ret);
    EXPECT_EQ(static_cast<int32_t>(val_out), 16);

    // 180 degrees = 3 radians
    unit_in.type = 0;
    hid::unit::SetSystem(hid::unit::System::eng_rotation, unit_in);
    hid::unit::SetLengthExp(unit_in, 1);
    unit_in.exp = 0;

    unit_out.type = 0;
    hid::unit::SetSystem(hid::unit::System::si_rotation, unit_out);
    hid::unit::SetLengthExp(unit_out, 1);
    unit_out.exp = 0;

    ret = hid::unit::ConvertUnits(180, unit_in, val_out, unit_out);
    EXPECT_TRUE(ret);
    EXPECT_EQ(static_cast<int32_t>(val_out), 3);

    // 3 radians = 171 degrees
    unit_in.type = 0;
    hid::unit::SetSystem(hid::unit::System::si_rotation, unit_in);
    hid::unit::SetLengthExp(unit_in, 1);
    unit_in.exp = 0;

    unit_out.type = 0;
    hid::unit::SetSystem(hid::unit::System::eng_rotation, unit_out);
    hid::unit::SetLengthExp(unit_out, 1);
    unit_out.exp = 0;

    ret = hid::unit::ConvertUnits(3, unit_in, val_out, unit_out);
    EXPECT_TRUE(ret);
    EXPECT_EQ(static_cast<int32_t>(val_out), 171);

    // Mass Converson Tests.
    // 1 slug = 14593 grams
    unit_in.type = 0;
    hid::unit::SetSystem(hid::unit::System::eng_linear, unit_in);
    hid::unit::SetMassExp(unit_in, 1);
    unit_in.exp = 0;

    unit_out.type = 0;
    hid::unit::SetSystem(hid::unit::System::si_linear, unit_out);
    hid::unit::SetMassExp(unit_out, 1);
    unit_out.exp = 0;

    ret = hid::unit::ConvertUnits(1, unit_in, val_out, unit_out);
    EXPECT_TRUE(ret);
    EXPECT_EQ(static_cast<int32_t>(val_out), 14593);

    // 200000 grams = 13 slugs
    unit_in.type = 0;
    hid::unit::SetSystem(hid::unit::System::si_linear, unit_in);
    hid::unit::SetMassExp(unit_in, 1);
    unit_in.exp = 0;

    unit_out.type = 0;
    hid::unit::SetSystem(hid::unit::System::eng_linear, unit_out);
    hid::unit::SetMassExp(unit_out, 1);
    unit_out.exp = 0;

    ret = hid::unit::ConvertUnits(200000, unit_in, val_out, unit_out);
    EXPECT_TRUE(ret);
    EXPECT_EQ(static_cast<int32_t>(val_out), 13);

    // Temperature Conversion Tests.
    // 32 F = 273 K
    unit_in.type = 0;
    hid::unit::SetSystem(hid::unit::System::eng_linear, unit_in);
    hid::unit::SetTemperatureExp(unit_in, 1);
    unit_in.exp = 0;

    unit_out.type = 0;
    hid::unit::SetSystem(hid::unit::System::si_linear, unit_out);
    hid::unit::SetTemperatureExp(unit_out, 1);
    unit_out.exp = 0;

    ret = hid::unit::ConvertUnits(32, unit_in, val_out, unit_out);
    EXPECT_TRUE(ret);
    EXPECT_EQ(static_cast<int32_t>(val_out), 273);

    // 273 K = 31 F
    unit_in.type = 0;
    hid::unit::SetSystem(hid::unit::System::si_linear, unit_in);
    hid::unit::SetTemperatureExp(unit_in, 1);
    unit_in.exp = 0;

    unit_out.type = 0;
    hid::unit::SetSystem(hid::unit::System::eng_linear, unit_out);
    hid::unit::SetTemperatureExp(unit_out, 1);
    unit_out.exp = 0;

    ret = hid::unit::ConvertUnits(273, unit_in, val_out, unit_out);
    EXPECT_TRUE(ret);
    EXPECT_EQ(static_cast<int32_t>(val_out), 31);

    // Misc Tests.
    // SlugUnits to Newtons (Force conversion).
    // 100 * 10^0 slug * in / seconds^2 == 37 * 10^5 g * cm / seconds^2
    unit_in.type = 0;
    hid::unit::SetSystem(hid::unit::System::eng_linear, unit_in);
    hid::unit::SetMassExp(unit_in, 1);
    hid::unit::SetLengthExp(unit_in, 1);
    hid::unit::SetTimeExp(unit_in, -2);
    unit_in.exp = 0;

    unit_out.type = 0;
    hid::unit::SetSystem(hid::unit::System::si_linear, unit_out);
    hid::unit::SetMassExp(unit_out, 1);
    hid::unit::SetLengthExp(unit_out, 1);
    hid::unit::SetTimeExp(unit_out, -2);
    unit_out.exp = 5;

    ret = hid::unit::ConvertUnits(100, unit_in, val_out, unit_out);
    EXPECT_TRUE(ret);
    EXPECT_EQ(static_cast<int32_t>(val_out), 37);

    // Failure Tests.
    // Can't convert between different units.
    unit_in.type = 0;
    hid::unit::SetSystem(hid::unit::System::eng_linear, unit_in);
    hid::unit::SetMassExp(unit_in, 1);
    unit_in.exp = 0;

    unit_out.type = 0;
    hid::unit::SetSystem(hid::unit::System::si_linear, unit_out);
    hid::unit::SetMassExp(unit_out, 2);
    unit_out.exp = 0;

    ret = hid::unit::ConvertUnits(1, unit_in, val_out, unit_out);
    EXPECT_FALSE(ret);

    // Can't convert between rotation and linear distance.
    unit_in.type = 0;
    hid::unit::SetSystem(hid::unit::System::eng_rotation, unit_in);
    hid::unit::SetLengthExp(unit_in, 1);
    unit_in.exp = 0;

    unit_out.type = 0;
    hid::unit::SetSystem(hid::unit::System::si_linear, unit_out);
    hid::unit::SetLengthExp(unit_out, 1);
    unit_out.exp = 0;

    ret = hid::unit::ConvertUnits(1, unit_in, val_out, unit_out);
    EXPECT_FALSE(ret);


    END_TEST;
}

BEGIN_TEST_CASE(hidparser_tests)
RUN_TEST(itemize_acer12_rpt1)
RUN_TEST(itemize_eve_tablet_rpt)
RUN_TEST(parse_boot_mouse)
RUN_TEST(parse_hp_mouse)
RUN_TEST(parse_push_pop)
RUN_TEST(parse_adaf_trinket)
RUN_TEST(parse_ps3_controller)
RUN_TEST(parse_acer12_touch)
RUN_TEST(parse_eve_tablet)
RUN_TEST(parse_asus_touch)
RUN_TEST(parse_eve_touchpad_v2)
RUN_TEST(usage_helper)
RUN_TEST(minmax_operators)
RUN_TEST(usage_operators)
RUN_TEST(extract_tests)
RUN_TEST(unit_tests)
END_TEST_CASE(hidparser_tests)

int main(int argc, char** argv) {
    bool success = unittest_run_all_tests(argc, argv);
    return success ? 0 : -1;
}
