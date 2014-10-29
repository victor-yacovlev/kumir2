#ifndef VM_BREAKPOINTS_TABLE_HPP
#define VM_BREAKPOINTS_TABLE_HPP

#include <map>
#include <utility>

#ifdef NO_UNICODE
    #include <string>
    typedef std::string String;
#else
    extern "C" {
        #include <wchar.h>
    }
    #include <string>
    typedef std::wstring String;
#endif

namespace VM {

typedef std::pair<uint8_t,uint32_t> BreakpointLocation;
typedef void* BreakpointCondition; // TODO condition support
typedef bool (*BreakpointConditionChecker)(const BreakpointCondition&);

struct BreakpointData {
    bool enabled;
    uint32_t ignoreCount;
    uint32_t hitCount;
    BreakpointCondition condition;

    inline explicit BreakpointData(): enabled(true), ignoreCount(0), hitCount(0), condition(0) {}
};

class BreakpointsTable {
public:
    inline bool isBreakpointHit(const uint8_t modId, const int lineNo, const BreakpointConditionChecker * conditionChecker) const;

    inline void reset();
    inline void registerSourceFileName(const String & sourceFileName, const uint8_t modId);
    inline const String & registeredSourceFileName(const uint8_t & modId) const;

    inline void removeAllBreakpoints();
    inline void insertOrChangeBreakpoint(const bool enabled, const String &fileName, const uint32_t lineNo, const uint32_t ignoreCount, const BreakpointCondition & condition);
    inline void removeBreakpoint(const String &fileName, const uint32_t lineNo);

private:
    typedef std::map<BreakpointLocation,BreakpointData> BreaksTable;
    typedef std::map<String,uint8_t> SourcesToIdsTable;
    typedef std::map<uint8_t,String> IdsToSourcesTable;

    BreaksTable breakpoints_;
    SourcesToIdsTable sourceToIds_;
    IdsToSourcesTable idsToSources_;
};

// ------------ INLINE IMPLEMENTATION

bool BreakpointsTable::isBreakpointHit(const uint8_t modId, const int lineNo, const BreakpointConditionChecker *conditionChecker) const
{
    if (-1 == lineNo)
        return false;

    bool result = false;
    const BreakpointLocation loc(modId, lineNo);
    BreaksTable::const_iterator locIt = breakpoints_.find(loc);
    if (breakpoints_.end() != locIt) {
        const BreakpointData & data = locIt->second;
        if (data.enabled) {
            result = true; // TODO check for hit count and condition
        }
    }
    return result;
}

void BreakpointsTable::reset()
{
    breakpoints_.clear();
    sourceToIds_.clear();
    idsToSources_.clear();
}

void BreakpointsTable::registerSourceFileName(const String & sourceFileName, const uint8_t modId)
{
    sourceToIds_[sourceFileName] = modId;
    idsToSources_[modId] = sourceFileName;
}

const String &BreakpointsTable::registeredSourceFileName(const uint8_t &modId) const
{
    return idsToSources_.at(modId);
}

void BreakpointsTable::removeAllBreakpoints()
{
    breakpoints_.clear();
}

void BreakpointsTable::insertOrChangeBreakpoint(const bool enabled, const String &fileName, const uint32_t lineNo, const uint32_t ignoreCount, const BreakpointCondition & condition)
{
    SourcesToIdsTable::const_iterator fnIt = sourceToIds_.find(fileName);
    if (sourceToIds_.end() != fnIt) {
        const uint8_t modId = fnIt->second;
        const BreakpointLocation loc(modId, lineNo);
        BreaksTable::iterator locIt = breakpoints_.find(loc);
        if (breakpoints_.end() != locIt) {
            BreakpointData & data = locIt->second;
            data.enabled = enabled;
            data.ignoreCount = ignoreCount;
            data.condition = condition;
        }
        else {
            BreakpointData data;
            data.enabled = enabled;
            data.ignoreCount = ignoreCount;
            data.condition = condition;
            breakpoints_[loc] = data;
        }
    }
}

void BreakpointsTable::removeBreakpoint(const String &fileName, const uint32_t lineNo)
{
    SourcesToIdsTable::const_iterator fnIt = sourceToIds_.find(fileName);
    if (sourceToIds_.end() != fnIt) {
        const uint8_t modId = fnIt->second;
        const BreakpointLocation loc(modId, lineNo);
        BreaksTable::iterator locIt = breakpoints_.find(loc);
        if (breakpoints_.end() != locIt) {
            breakpoints_.erase(locIt);
        }
    }
}


}

#endif // VM_BREAKPOINTS_TABLE_HPP
