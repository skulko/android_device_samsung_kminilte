/*
   Copyright (c) 2013, The Linux Foundation. All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are
   met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the following
      disclaimer in the documentation and/or other materials provided
      with the distribution.
    * Neither the name of The Linux Foundation nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
   WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
   ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
   BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
   BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
   WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
   OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define _REALLY_INCLUDE_SYS__SYSTEM_PROPERTIES_H_
#include <sys/_system_properties.h>

#include <android-base/file.h>
#include <android-base/logging.h>
#include <android-base/properties.h>
#include <android-base/strings.h>

#include "property_service.h"
#include "vendor_init.h"

#define LOG_TAG "init_properties"
#include <cutils/log.h>

#define SERIAL_NUMBER_FILE "/efs/FactoryApp/serial_no"

using android::base::GetProperty;
using android::base::ReadFileToString;
using android::base::Trim;
using android::init::property_set;


// copied from build/tools/releasetools/ota_from_target_files.py
// but with "." at the end and empty entry
std::vector<std::string> ro_product_props_default_source_order = {
    "",
    "product.",
    "product_services.",
    "odm.",
    "vendor.",
    "system.",
};

void property_override(char const prop[], char const value[], bool add = true)
{
    auto pi = (prop_info *) __system_property_find(prop);

    if (pi != nullptr) {
        __system_property_update(pi, value, strlen(value));
    } else if (add) {
        __system_property_add(prop, strlen(prop), value, strlen(value));
    }
}

void vendor_load_properties()
{
    std::string bootloader = android::base::GetProperty("ro.bootloader", "");

    char const *serial_number_file = SERIAL_NUMBER_FILE;
    std::string serial_number;

    if (ReadFileToString(serial_number_file, &serial_number)) {
        serial_number = Trim(serial_number);
        property_override("ro.serialno", serial_number.c_str());
    }

    const auto set_ro_product_prop = [](const std::string &source,
            const std::string &prop, const std::string &value) {
        auto prop_name = "ro.product." + source + prop;
        property_override(prop_name.c_str(), value.c_str(), false);
    };

    if (bootloader.find("G800F") != std::string::npos) {
        /* kminiltexx */
        for (const auto &source : ro_product_props_default_source_order) {
            set_ro_product_prop(source, "fingerprint", "samsung/kminiltexx/kminilte:6.0.1/MMB29K/G800FXXU1CRG3:user/release-keys");
            set_ro_product_prop(source, "device", "kminiltexx");
            set_ro_product_prop(source, "model", "SM-G800F");
            set_ro_product_prop(source, "name", "kminiltexx");
        }
        property_override("ro.build.description", "kminiltexx-user 6.0.1 MMB29K G800FXXU1CRG3 release-keys");
        property_override("ro.build.product", "kminiltexx");
    } else if (bootloader.find("G800Y") != std::string::npos) {
        /* kminiltedv */
        for (const auto &source : ro_product_props_default_source_order) {
            set_ro_product_prop(source, "fingerprint", "samsung/kminiltedv/kminilte:5.1.1/LMY49J/G800YUIS1BPL1:user/release-keys");
            set_ro_product_prop(source, "device", "kminiltedv");
            set_ro_product_prop(source, "model", "SM-G800Y");
            set_ro_product_prop(source, "name", "kminiltedv");
        }
        property_override("ro.build.description", "kminiltedv-user 5.1.1 LMY49J G800YUIS1BPL1 release-keys");
        property_override("ro.build.product", "kminiltedv");
    } else if (bootloader.find("G800M") != std::string::npos) {
        /* kminilteub */
        for (const auto &source : ro_product_props_default_source_order) {
            set_ro_product_prop(source, "fingerprint", "samsung/kminilteub/kminilte:6.0.1/MMB29K/G800MUBU1CPL1:user/release-keys");
            set_ro_product_prop(source, "device", "kminilteub");
            set_ro_product_prop(source, "model", "SM-G800M");
            set_ro_product_prop(source, "name", "kminilteub");
        }
        property_override("ro.build.description", "kminilteub-user 6.0.1 MMB29K G800MUBU1CPL1 release-keys");
        property_override("ro.build.product", "kminilteub");
    }

    std::string device = android::base::GetProperty("ro.product.device", "");
    ALOGI("Found bootloader id %s setting build properties for %s device\n", bootloader.c_str(), device.c_str());
}

