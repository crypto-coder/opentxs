/************************************************************
 *
 *                 OPEN TRANSACTIONS
 *
 *       Financial Cryptography and Digital Cash
 *       Library, Protocol, API, Server, CLI, GUI
 *
 *       -- Anonymous Numbered Accounts.
 *       -- Untraceable Digital Cash.
 *       -- Triple-Signed Receipts.
 *       -- Cheques, Vouchers, Transfers, Inboxes.
 *       -- Basket Currencies, Markets, Payment Plans.
 *       -- Signed, XML, Ricardian-style Contracts.
 *       -- Scripted smart contracts.
 *
 *  EMAIL:
 *  fellowtraveler@opentransactions.org
 *
 *  WEBSITE:
 *  http://www.opentransactions.org/
 *
 *  -----------------------------------------------------
 *
 *   LICENSE:
 *   This Source Code Form is subject to the terms of the
 *   Mozilla Public License, v. 2.0. If a copy of the MPL
 *   was not distributed with this file, You can obtain one
 *   at http://mozilla.org/MPL/2.0/.
 *
 *   DISCLAIMER:
 *   This program is distributed in the hope that it will
 *   be useful, but WITHOUT ANY WARRANTY; without even the
 *   implied warranty of MERCHANTABILITY or FITNESS FOR A
 *   PARTICULAR PURPOSE.  See the Mozilla Public License
 *   for more details.
 *
 ************************************************************/

#include <opentxs/core/stdafx.hpp>

#include <opentxs/core/OTSettings.hpp>
#include <opentxs/core/Log.hpp>
#include <opentxs/core/util/OTPaths.hpp>

#include "simpleini/SimpleIni.h"
#include <cinttypes>

namespace opentxs
{

class OTSettings::OTSettingsPvt
{
private:
    OTSettingsPvt(const OTSettingsPvt&);
    OTSettingsPvt& operator=(const OTSettingsPvt&);

public:
    CSimpleIniA iniSimple;

    OTSettingsPvt()
        : iniSimple()
    {
    }
};

bool OTSettings::Load(const String& strConfigurationFileExactPath)
{
    if (!strConfigurationFileExactPath.Exists()) {
        otErr << __FUNCTION__ << ": Error: "
              << "strConfigurationFileExactPath"
              << " is Empty!\n";
        return false;
    }

    bool bFolderCreated(false);
    if (!OTPaths::BuildFilePath(strConfigurationFileExactPath,
                                bFolderCreated)) {
        OT_FAIL;
    };

    if (!IsEmpty()) {
        otErr << __FUNCTION__ << ": Bad: "
              << "p_Settings"
              << " is not Empty!\n";
        OT_FAIL;
    }

    int64_t lFilelength;
    if (!OTPaths::FileExists(strConfigurationFileExactPath,
                             lFilelength)) // we don't have a config file, lets
                                           // create a blank one first.
    {
        pvt->iniSimple.Reset(); // clean the config.

        SI_Error rc = pvt->iniSimple.SaveFile(
            strConfigurationFileExactPath.Get()); // save a new file.
        if (0 > rc) return false;                 // error!

        pvt->iniSimple.Reset(); // clean the config (again).
    }

    SI_Error rc = pvt->iniSimple.LoadFile(strConfigurationFileExactPath.Get());
    if (0 > rc)
        return false;
    else
        return true;
}

bool OTSettings::Save(const String& strConfigurationFileExactPath)
{
    if (!strConfigurationFileExactPath.Exists()) {
        otErr << __FUNCTION__ << ": Error: "
              << "strConfigurationFileExactPath"
              << " is Empty!\n";
        return false;
    }

    SI_Error rc = pvt->iniSimple.SaveFile(strConfigurationFileExactPath.Get());
    if (0 > rc)
        return false;
    else
        return true;
}

bool OTSettings::LogChange_str(const String& strSection, const String& strKey,
                               const String& strValue)
{
    if (!strSection.Exists()) {
        otErr << __FUNCTION__ << ": Error: "
              << "strSection"
              << " is Empty!\n";
        OT_FAIL;
    }
    if (!strKey.Exists()) {
        otErr << __FUNCTION__ << ": Error: "
              << "strKey"
              << " is Empty!\n";
        OT_FAIL;
    }

    const char* const szValue = (strValue.Exists() && !strValue.Compare(""))
                                    ? strValue.Get()
                                    : "nullptr";

    String strCategory, strOption;
    if (!Log::StringFill(strCategory, strSection.Get(), 12)) return false;
    if (!Log::StringFill(strOption, strKey.Get(), 30, " to:")) return false;

    otWarn << "Setting " << strCategory << " " << strOption << " " << szValue
           << " \n";
    return true;
}

OTSettings::OTSettings(const String& strConfigFilePath)
    : pvt(new OTSettingsPvt())
    , b_Loaded(false)
    , m_strConfigurationFileExactPath(strConfigFilePath)
{
    if (!m_strConfigurationFileExactPath.Exists()) {
        otErr << __FUNCTION__ << ": Error: "
              << "m_strConfigurationFileExactPath"
              << " is Empty!\n";
        OT_FAIL;
    }
}

void OTSettings::SetConfigFilePath(const String& strConfigFilePath)
{
    m_strConfigurationFileExactPath.Set(strConfigFilePath.Get());
}

bool OTSettings::HasConfigFilePath()
{
    return m_strConfigurationFileExactPath.Exists();
}

OTSettings::OTSettings()
    : pvt(new OTSettingsPvt())
    , b_Loaded(false)
{
}

OTSettings::~OTSettings()
{
}

bool OTSettings::Load()
{
    b_Loaded = false;

    if (Load(m_strConfigurationFileExactPath)) {
        b_Loaded = true;
        return true;
    }
    else
        return false;
}

bool OTSettings::Save()
{
    return Save(m_strConfigurationFileExactPath);
}

const bool& OTSettings::IsLoaded() const
{
    return b_Loaded;
}

bool OTSettings::Reset()
{
    b_Loaded = false;
    pvt->iniSimple.Reset();
    return true;
}

bool OTSettings::IsEmpty() const
{
    return pvt->iniSimple.IsEmpty();
}

bool OTSettings::Check_str(const String& strSection, const String& strKey,
                           String& out_strResult, bool& out_bKeyExist) const
{
    if (!strSection.Exists()) {
        otErr << __FUNCTION__ << ": Error: "
              << "strSection"
              << " is Empty!\n";
        OT_FAIL;
    }
    if (strSection.Compare("")) {
        otErr << __FUNCTION__ << ": Error: "
              << "strSection"
              << " is Blank!\n";
        OT_FAIL;
    }

    if (!strKey.Exists()) {
        otErr << __FUNCTION__ << ": Error: "
              << "strKey"
              << " is Empty!\n";
        OT_FAIL;
    }
    if (strKey.Compare("")) {
        otErr << __FUNCTION__ << ": Error: "
              << "strKey"
              << " is Blank!\n";
        OT_FAIL;
    }

    const char* szVar =
        pvt->iniSimple.GetValue(strSection.Get(), strKey.Get(), nullptr);
    String strVar(szVar);

    if (strVar.Exists() && !strVar.Compare("")) {
        out_bKeyExist = true;
        out_strResult = strVar;
    }
    else {
        out_bKeyExist = false;
        out_strResult = "";
    }

    return true;
}

bool OTSettings::Check_long(const String& strSection, const String& strKey,
                            int64_t& out_lResult, bool& out_bKeyExist) const
{
    if (!strSection.Exists()) {
        otErr << __FUNCTION__ << ": Error: "
              << "strSection"
              << " is Empty!\n";
        OT_FAIL;
    }
    if (strSection.Compare("")) {
        otErr << __FUNCTION__ << ": Error: "
              << "strSection"
              << " is Blank!\n";
        OT_FAIL;
    }

    if (!strKey.Exists()) {
        otErr << __FUNCTION__ << ": Error: "
              << "strKey"
              << " is Empty!\n";
        OT_FAIL;
    }
    if (strKey.Compare("")) {
        otErr << __FUNCTION__ << ": Error: "
              << "strKey"
              << " is Blank!\n";
        OT_FAIL;
    }

    const char* szVar =
        pvt->iniSimple.GetValue(strSection.Get(), strKey.Get(), nullptr);
    String strVar(szVar);

    if (strVar.Exists() && !strVar.Compare("")) {
        out_bKeyExist = true;
        out_lResult =
            pvt->iniSimple.GetLongValue(strSection.Get(), strKey.Get(), 0);
    }
    else {
        out_bKeyExist = false;
        out_lResult = 0;
    }

    return true;
}

bool OTSettings::Check_bool(const String& strSection, const String& strKey,
                            bool& out_bResult, bool& out_bKeyExist) const
{
    if (!strSection.Exists()) {
        otErr << __FUNCTION__ << ": Error: "
              << "strSection"
              << " is Empty!\n";
        OT_FAIL;
    }
    if (strSection.Compare("")) {
        otErr << __FUNCTION__ << ": Error: "
              << "strSection"
              << " is Blank!\n";
        OT_FAIL;
    }

    if (!strKey.Exists()) {
        otErr << __FUNCTION__ << ": Error: "
              << "strKey"
              << " is Empty!\n";
        OT_FAIL;
    }
    if (strKey.Compare("")) {
        otErr << __FUNCTION__ << ": Error: "
              << "strKey"
              << " is Blank!\n";
        OT_FAIL;
    }

    const char* szVar =
        pvt->iniSimple.GetValue(strSection.Get(), strKey.Get(), nullptr);
    String strVar(szVar);

    if (strVar.Exists() &&
        (strVar.Compare("false") || strVar.Compare("true"))) {
        out_bKeyExist = true;
        if (strVar.Compare("true"))
            out_bResult = true;
        else
            out_bResult = false;
    }
    else {
        out_bKeyExist = false;
        out_bResult = false;
    }

    return true;
}

bool OTSettings::Set_str(const String& strSection, const String& strKey,
                         const String& strValue, bool& out_bNewOrUpdate,
                         const String& strComment)
{
    if (!strSection.Exists()) {
        otErr << __FUNCTION__ << ": Error: "
              << "strSection"
              << " is Empty!\n";
        OT_FAIL;
    }
    if (strSection.Compare("")) {
        otErr << __FUNCTION__ << ": Error: "
              << "strSection"
              << " is Blank!\n";
        OT_FAIL;
    }

    if (!strKey.Exists()) {
        otErr << __FUNCTION__ << ": Error: "
              << "strKey"
              << " is Empty!\n";
        OT_FAIL;
    }
    if (strKey.Compare("")) {
        otErr << __FUNCTION__ << ": Error: "
              << "strKey"
              << " is Blank!\n";
        OT_FAIL;
    }

    // if (nullptr == m_strConfigurationFileExactPath){ otErr << "%s: Error:
    // %s is a nullptr!\n", __FUNCTION__, "p_iniSimple"); OT_FAIL; }

    const char* const szValue =
        (strValue.Exists() && !strValue.Compare("")) ? strValue.Get() : nullptr;
    const char* const szComment =
        (strComment.Exists() && !strComment.Compare("")) ? strComment.Get()
                                                         : nullptr;

    String strOldValue, strNewValue;
    bool bOldKeyExist, bNewKeyExist;

    // Check if Old Key exists.
    if (!Check_str(strSection, strKey, strOldValue, bOldKeyExist)) return false;

    if (bOldKeyExist) {
        if (strValue.Compare(strOldValue)) {
            out_bNewOrUpdate = false;
            return true;
        }
    }

    // Log to Output Setting Change
    if (!LogChange_str(strSection, strKey, strValue)) return false;

    // Set New Value
    SI_Error rc = pvt->iniSimple.SetValue(strSection.Get(), strKey.Get(),
                                          szValue, szComment, true);
    if (0 > rc) return false;

    if (nullptr == szValue) // We set the key's value to null, thus removing it.
    {
        if (bOldKeyExist)
            out_bNewOrUpdate = true;
        else
            out_bNewOrUpdate = false;

        return true;
    }

    // Check if the new value is the same as intended.
    if (!Check_str(strSection, strKey, strNewValue, bNewKeyExist)) return false;

    if (bNewKeyExist) {
        if (strValue.Compare(strNewValue)) {
            // Success
            out_bNewOrUpdate = true;
            return true;
        }
    }

    // If we get here, error!
    OT_FAIL;
}

bool OTSettings::Set_long(const String& strSection, const String& strKey,
                          const int64_t& lValue, bool& out_bNewOrUpdate,
                          const String& strComment)
{
    if (!strSection.Exists()) {
        otErr << __FUNCTION__ << ": Error: "
              << "strSection"
              << " is Empty!\n";
        OT_FAIL;
    }
    if (strSection.Compare("")) {
        otErr << __FUNCTION__ << ": Error: "
              << "strSection"
              << " is Blank!\n";
        OT_FAIL;
    }

    if (!strKey.Exists()) {
        otErr << __FUNCTION__ << ": Error: "
              << "strKey"
              << " is Empty!\n";
        OT_FAIL;
    }
    if (strKey.Compare("")) {
        otErr << __FUNCTION__ << ": Error: "
              << "strKey"
              << " is Blank!\n";
        OT_FAIL;
    }

    String strValue;
    strValue.Format("%" PRId64, lValue);

    const char* const szComment =
        (strComment.Exists() && !strComment.Compare("")) ? strComment.Get()
                                                         : nullptr;

    String strOldValue, strNewValue;
    bool bOldKeyExist, bNewKeyExist;

    // Check if Old Key exists.
    if (!Check_str(strSection, strKey, strOldValue, bOldKeyExist)) return false;

    if (bOldKeyExist) {
        if (strValue.Compare(strOldValue)) {
            out_bNewOrUpdate = false;
            return true;
        }
    }

    // Log to Output Setting Change
    if (!LogChange_str(strSection, strKey, strValue)) return false;

    // Set New Value
    SI_Error rc = pvt->iniSimple.SetLongValue(strSection.Get(), strKey.Get(),
                                              lValue, szComment, false, true);
    if (0 > rc) return false;

    // Check if the new value is the same as intended.
    if (!Check_str(strSection, strKey, strNewValue, bNewKeyExist)) return false;

    if (bNewKeyExist) {
        if (strValue.Compare(strNewValue)) {
            // Success
            out_bNewOrUpdate = true;
            return true;
        }
    }

    // If we get here, error!
    OT_FAIL;
}

bool OTSettings::Set_bool(const String& strSection, const String& strKey,
                          const bool& bValue, bool& out_bNewOrUpdate,
                          const String& strComment)
{
    if (!strSection.Exists()) {
        otErr << __FUNCTION__ << ": Error: "
              << "strSection"
              << " is Empty!\n";
        OT_FAIL;
    }
    if (!strKey.Exists()) {
        otErr << __FUNCTION__ << ": Error: "
              << "strKey"
              << " is Empty!\n";
        OT_FAIL;
    }
    const String strValue(bValue ? "true" : "false");

    return Set_str(strSection, strKey, strValue, out_bNewOrUpdate, strComment);
}

bool OTSettings::CheckSetSection(const String& strSection,
                                 const String& strComment,
                                 bool& out_bIsNewSection)
{
    if (!strSection.Exists()) {
        otErr << __FUNCTION__ << ": Error: "
              << "strSection"
              << " is Empty!\n";
        OT_FAIL;
    }
    if (!strComment.Exists()) {
        otErr << __FUNCTION__ << ": Error: "
              << "strComment"
              << " is Empty!\n";
        OT_FAIL;
    }

    const char* const szComment =
        (strComment.Exists() && !strComment.Compare("")) ? strComment.Get()
                                                         : nullptr;

    const int64_t lSectionSize =
        pvt->iniSimple.GetSectionSize(strSection.Get());

    if (1 > lSectionSize) {
        out_bIsNewSection = true;
        SI_Error rc = pvt->iniSimple.SetValue(strSection.Get(), nullptr,
                                              nullptr, szComment, false);
        if (0 > rc) return false;
    }
    else {
        out_bIsNewSection = false;
    }
    return true;
}

bool OTSettings::CheckSet_str(const String& strSection, const String& strKey,
                              const String& strDefault, String& out_strResult,
                              bool& out_bIsNew, const String& strComment)
{
    if (!strSection.Exists()) {
        otErr << __FUNCTION__ << ": Error: "
              << "strSection"
              << " is Empty!\n";
        OT_FAIL;
    }
    if (!strKey.Exists()) {
        otErr << __FUNCTION__ << ": Error: "
              << "strKey"
              << " is Empty!\n";
        OT_FAIL;
    }

    const char* const szDefault =
        (strDefault.Exists() && !strDefault.Compare("")) ? strDefault.Get()
                                                         : nullptr;

    String strTempResult;
    bool bKeyExist;
    if (!Check_str(strSection, strKey, strTempResult, bKeyExist)) return false;

    if (bKeyExist) {
        // Already have a key, lets use it's value.
        out_bIsNew = false;
        out_strResult = strTempResult;
        return true;
    }
    else {
        bool bNewKeyCheck;
        if (!Set_str(strSection, strKey, strDefault, bNewKeyCheck, strComment))
            return false;

        if (nullptr == szDefault) // The Default is to have no key.
        {
            // Success
            out_bIsNew = false;
            out_strResult = "";
            return true;
        }

        if (bNewKeyCheck) {
            // Success
            out_bIsNew = true;
            out_strResult = strDefault;
            return true;
        }
    }

    // If we get here, error!
    OT_FAIL;
}

bool OTSettings::CheckSet_long(const String& strSection, const String& strKey,
                               const int64_t& lDefault, int64_t& out_lResult,
                               bool& out_bIsNew, const String& strComment)
{
    if (!strSection.Exists()) {
        otErr << __FUNCTION__ << ": Error: "
              << "strSection"
              << " is Empty!\n";
        OT_FAIL;
    }
    if (!strKey.Exists()) {
        otErr << __FUNCTION__ << ": Error: "
              << "strKey"
              << " is Empty!\n";
        OT_FAIL;
    }

    int64_t lTempResult;
    bool bKeyExist;
    if (!Check_long(strSection, strKey, lTempResult, bKeyExist)) return false;

    if (bKeyExist) {
        // Already have a key, lets use it's value.
        out_bIsNew = false;
        out_lResult = lTempResult;
        return true;
    }
    else {
        bool bNewKeyCheck;
        if (!Set_long(strSection, strKey, lDefault, bNewKeyCheck, strComment))
            return false;
        if (bNewKeyCheck) {
            // Success
            out_bIsNew = true;
            out_lResult = lDefault;
            return true;
        }
    }

    // If we get here, error!
    OT_FAIL;
}

bool OTSettings::CheckSet_bool(const String& strSection, const String& strKey,
                               const bool& bDefault, bool& out_bResult,
                               bool& out_bIsNew, const String& strComment)
{
    if (!strSection.Exists()) {
        otErr << __FUNCTION__ << ": Error: "
              << "strSection"
              << " is Empty!\n";
        OT_FAIL;
    }
    if (!strKey.Exists()) {
        otErr << __FUNCTION__ << ": Error: "
              << "strKey"
              << " is Empty!\n";
        OT_FAIL;
    }

    bool bKeyExist, bTempResult;
    if (!Check_bool(strSection, strKey, bTempResult, bKeyExist)) return false;

    if (bKeyExist) {
        // Already have a key, lets use it's value.
        out_bIsNew = false;
        out_bResult = bTempResult;
        return true;
    }
    else {
        bool bNewKeyCheck;
        if (!Set_bool(strSection, strKey, bDefault, bNewKeyCheck, strComment))
            return false;
        if (bNewKeyCheck) {
            // Success
            out_bIsNew = true;
            out_bResult = bDefault;
            return true;
        }
    }

    // If we get here, error!
    OT_FAIL;
}

bool OTSettings::SetOption_bool(const String& strSection, const String& strKey,
                                bool& bVariableName)
{
    bool bNewOrUpdate;
    return CheckSet_bool(strSection, strKey, bVariableName, bVariableName,
                         bNewOrUpdate);
}

} // namespace opentxs
