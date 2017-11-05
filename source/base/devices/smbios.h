//
// PROJECT:         Aspia Remote Desktop
// FILE:            base/devices/smbios.h
// LICENSE:         Mozilla Public License Version 2.0
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#ifndef _ASPIA_BASE__DEVICES__SMBIOS_H
#define _ASPIA_BASE__DEVICES__SMBIOS_H

#include "base/macros.h"

#include <memory>
#include <string>
#include <list>

namespace aspia {

class SMBios
{
    class TableEnumeratorImpl;
    struct Data;

public:
    static std::unique_ptr<SMBios> Create(std::unique_ptr<uint8_t[]> data, size_t data_size);

    uint8_t GetMajorVersion() const;
    uint8_t GetMinorVersion() const;

    template <class T>
    class TableEnumerator
    {
    public:
        TableEnumerator(const SMBios& smbios)
            : impl_(reinterpret_cast<const Data*>(smbios.data_.get()), T::TABLE_TYPE)
        {
            // Nothing
        }

        bool IsAtEnd() const { return impl_.IsAtEnd(); }
        void Advance() { impl_.Advance(T::TABLE_TYPE); }
        T GetTable() const { return T(TableReader(impl_.GetSMBiosData(), impl_.GetTableData())); }

    private:
        TableEnumeratorImpl impl_;
        DISALLOW_COPY_AND_ASSIGN(TableEnumerator);
    };

    class TableReader
    {
    public:
        TableReader(const TableReader& other);
        TableReader(const Data* smbios, const uint8_t* table);

        uint8_t GetMajorVersion() const { return smbios_->smbios_major_version; }
        uint8_t GetMinorVersion() const { return smbios_->smbios_minor_version; }
        uint8_t GetByte(uint8_t offset) const;
        uint16_t GetWord(uint8_t offset) const;
        uint32_t GetDword(uint8_t offset) const;
        uint64_t GetQword(uint8_t offset) const;
        std::string GetString(uint8_t offset) const;
        const uint8_t* GetPointer(uint8_t offset) const;
        uint8_t GetTableLength() const;

    private:
        const Data* smbios_;
        const uint8_t* table_;
    };

    class BiosTable
    {
    public:
        enum : uint8_t { TABLE_TYPE = 0x00 };

        using Feature = std::pair<std::string, bool>;
        using FeatureList = std::list<Feature>;

        std::string GetManufacturer() const;
        std::string GetVersion() const;
        std::string GetDate() const;
        int GetSize() const; // In kB.
        std::string GetBiosRevision() const;
        std::string GetFirmwareRevision() const;
        std::string GetAddress() const;
        int GetRuntimeSize() const; // In bytes.
        FeatureList GetCharacteristics() const;

    private:
        friend class TableEnumerator<BiosTable>;
        BiosTable(const TableReader& reader);

        TableReader reader_;
    };

    class SystemTable
    {
    public:
        enum : uint8_t { TABLE_TYPE = 0x01 };

        std::string GetManufacturer() const;
        std::string GetProductName() const;
        std::string GetVersion() const;
        std::string GetSerialNumber() const;
        std::string GetUUID() const;
        std::string GetWakeupType() const;
        std::string GetSKUNumber() const;
        std::string GetFamily() const;

    private:
        friend class TableEnumerator<SystemTable>;
        SystemTable(const TableReader& reader);

        TableReader reader_;
    };

    class BaseboardTable
    {
    public:
        enum : uint8_t { TABLE_TYPE = 0x02 };

        using Feature = std::pair<std::string, bool>;
        using FeatureList = std::list<Feature>;

        enum class BoardType
        {
            UNKNOWN                      = 0,
            OTHER                        = 1,
            SERVER_BLADE                 = 2,
            CONNECTIVITY_SWITCH          = 3,
            SYSTEM_MANAGEMENT_MODULE     = 4,
            PROCESSOR_MODULE             = 5,
            IO_MODULE                    = 6,
            MEMORY_MODULE                = 7,
            DAUGHTER_BOARD               = 8,
            MOTHERBOARD                  = 9,
            PROCESSOR_PLUS_MEMORY_MODULE = 10,
            PROCESSOR_PLUS_IO_MODULE     = 11,
            INTERCONNECT_BOARD           = 12
        };

        std::string GetManufacturer() const;
        std::string GetProductName() const;
        std::string GetVersion() const;
        std::string GetSerialNumber() const;
        std::string GetAssetTag() const;
        FeatureList GetFeatures() const;
        std::string GetLocationInChassis() const;
        BoardType GetBoardType() const;

    private:
        friend class TableEnumerator<BaseboardTable>;
        BaseboardTable(const TableReader& reader);

        TableReader reader_;
    };

    class ChassisTable
    {
    public:
        enum : uint8_t { TABLE_TYPE = 0x03 };

        enum class Status
        {
            UNKNOWN         = 0,
            OTHER           = 1,
            SAFE            = 2,
            WARNING         = 3,
            CRITICAL        = 4,
            NON_RECOVERABLE = 5
        };

        enum class SecurityStatus
        {
            UNKNOWN                       = 0,
            OTHER                         = 1,
            NONE                          = 2,
            EXTERNAL_INTERFACE_LOCKED_OUT = 3,
            EXTERNAL_INTERFACE_ENABLED    = 4
        };

        enum class Type
        {
            UNKNOWN               = 0,
            OTHER                 = 1,
            DESKTOP               = 2,
            LOW_PROFILE_DESKTOP   = 3,
            PIZZA_BOX             = 4,
            MINI_TOWER            = 5,
            TOWER                 = 6,
            PORTABLE              = 7,
            LAPTOP                = 8,
            NOTEBOOK              = 9,
            HAND_HELD             = 10,
            DOCKING_STATION       = 11,
            ALL_IN_ONE            = 12,
            SUB_NOTEBOOK          = 13,
            SPACE_SAVING          = 14,
            LUNCH_BOX             = 15,
            MAIN_SERVER_CHASSIS   = 16,
            EXPANSION_CHASSIS     = 17,
            SUB_CHASSIS           = 18,
            BUS_EXPANSION_CHASSIS = 19,
            PERIPHERIAL_CHASSIS   = 20,
            RAID_CHASSIS          = 21,
            RACK_MOUNT_CHASSIS    = 22,
            SEALED_CASE_PC        = 23,
            MULTI_SYSTEM_CHASSIS  = 24,
            COMPACT_PCI           = 25,
            ADVANCED_TCA          = 26,
            BLADE                 = 27,
            BLADE_ENCLOSURE       = 28
        };

        std::string GetManufacturer() const;
        std::string GetVersion() const;
        std::string GetSerialNumber() const;
        std::string GetAssetTag() const;
        Type GetType() const;
        Status GetOSLoadStatus() const;
        Status GetPowerSourceStatus() const;
        Status GetTemperatureStatus() const;
        SecurityStatus GetSecurityStatus() const;
        int GetHeight() const; // In Units.
        int GetNumberOfPowerCords() const;

    private:
        friend class TableEnumerator<ChassisTable>;
        ChassisTable(const TableReader& reader);

        TableReader reader_;
    };

private:
    SMBios(std::unique_ptr<uint8_t[]> data, size_t data_size);
    static int GetTableCount(const uint8_t* table_data, uint32_t length);

    static const size_t kMaxDataSize = 0xFA00; // 64K

    struct Data
    {
        uint8_t used_20_calling_method;
        uint8_t smbios_major_version;
        uint8_t smbios_minor_version;
        uint8_t dmi_revision;
        uint32_t length;
        uint8_t smbios_table_data[kMaxDataSize];
    };

    class TableEnumeratorImpl
    {
    public:
        TableEnumeratorImpl(const Data* data, uint8_t type);

        bool IsAtEnd() const;
        void Advance(uint8_t type);
        const Data* GetSMBiosData() const;
        const uint8_t* GetTableData() const;

    private:
        const Data* data_;
        const uint8_t* start_;
        const uint8_t* next_;
        const uint8_t* end_;
        const uint8_t* current_;
        DISALLOW_COPY_AND_ASSIGN(TableEnumeratorImpl);
    };

    std::unique_ptr<uint8_t[]> data_;
    const size_t data_size_;

    DISALLOW_COPY_AND_ASSIGN(SMBios);
};

} // namespace aspia

#endif // _ASPIA_BASE__DEVICES__SMBIOS_H