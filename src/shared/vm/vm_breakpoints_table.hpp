#ifndef VM_BREAKPOINTS_TABLE_HPP
#define VM_BREAKPOINTS_TABLE_HPP

#include <map>
#include <utility>
extern "C" {
    #include <wchar.h>
}
#include <string>
// #ifdef NO_UNICODE
//     #include <string>
//     typedef std::string std::wstring;
// #else
//     extern "C" {
//         #include <wchar.h>
//     }
//     #include <string>
//     typedef std::wstring std::wstring;
// #endif

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
    inline bool processBreakpointHit(const uint8_t modId, const int lineNo, const BreakpointConditionChecker * conditionChecker);

    inline void reset();
    inline void registerSourceFileName(const std::wstring & sourceFileName, const uint8_t modId);
    inline const std::wstring & registeredSourceFileName(const uint8_t & modId) const;

    inline void removeAllBreakpoints();
    inline void insertOrChangeBreakpoint(const bool enabled, const std::wstring &fileName, const uint32_t lineNo, const uint32_t ignoreCount, const BreakpointCondition & condition);
    inline void insertSingleHitBreakpoint(const std::wstring &fileName, uint32_t lineNo);
    inline void removeBreakpoint(const std::wstring &fileName, const uint32_t lineNo);

private:
    typedef std::map<BreakpointLocation,BreakpointData> BreaksTable;
    typedef std::map<std::wstring,uint8_t> SourcesToIdsTable;
    typedef std::map<uint8_t,std::wstring> IdsToSourcesTable;

    BreaksTable breakpoints_;
    BreaksTable singleHits_;
    SourcesToIdsTable sourceToIds_;
    IdsToSourcesTable idsToSources_;
};

// ------------ INLINE IMPLEMENTATION

bool BreakpointsTable::processBreakpointHit(const uint8_t modId, const int lineNo, const BreakpointConditionChecker *conditionChecker)
{
    if (-1 == lineNo)
        return false;

    bool result = false;
    const BreakpointLocation loc(modId, lineNo);
    BreaksTable::iterator shitIt = singleHits_.find(loc);
    if (singleHits_.end() != shitIt) {
        result = true;
        singleHits_.erase(shitIt);
    }
    if (!result) {
        BreaksTable::const_iterator locIt = breakpoints_.find(loc);
        if (breakpoints_.end() != locIt) {
            const BreakpointData & data = locIt->second;
            if (data.enabled) {
                result = true; // TODO check for hit count and condition
            }
        }
    }
    return result;
}

void BreakpointsTable::reset()
{
    breakpoints_.clear();
    singleHits_.clear();
    sourceToIds_.clear();
    idsToSources_.clear();
}

void BreakpointsTable::registerSourceFileName(const std::wstring & sourceFileName, const uint8_t modId)
{
    sourceToIds_[sourceFileName] = modId;
    idsToSources_[modId] = sourceFileName;
}

const std::wstring &BreakpointsTable::registeredSourceFileName(const uint8_t &modId) const
{
    return idsToSources_.at(modId);
}

void BreakpointsTable::removeAllBreakpoints()
{
    singleHits_.clear();
    breakpoints_.clear();
}

void BreakpointsTable::insertOrChangeBreakpoint(const bool enabled, const std::wstring &fileName, const uint32_t lineNo, const uint32_t ignoreCount, const BreakpointCondition & condition)
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

void BreakpointsTable::insertSingleHitBreakpoint(const std::wstring &fileName, uint32_t lineNo)
{
    SourcesToIdsTable::const_iterator fnIt = sourceToIds_.find(fileName);
    if (sourceToIds_.end() != fnIt) {
        const uint8_t modId = fnIt->second;
        const BreakpointLocation loc(modId, lineNo);
        BreakpointData data;
        data.condition = 0;
        data.ignoreCount = 0;
        data.hitCount = 0;
        data.enabled = true;
        singleHits_[loc] = data;
    }
}

void BreakpointsTable::removeBreakpoint(const std::wstring &fileName, const uint32_t lineNo)
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
