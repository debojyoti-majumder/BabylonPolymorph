/********************************************************
*                                                       *
*   Copyright (C) Microsoft. All rights reserved.       *
*                                                       *
********************************************************/

#include "FrameworkPch.h"
#include <Framework/PerformanceLoggingProviders.h>

//-------------------------------------------------------------------------------------------------

Babylon::Utils::TimePoint Babylon::Utils::DefaultTimeProvider::GetCurrentTime() const
{
    return std::chrono::high_resolution_clock::now();
}

//-------------------------------------------------------------------------------------------------

size_t Babylon::Utils::TraceInfo_Base::GetMemoryFootprint() const
{
    // Get base size of all primatives/pointers
    size_t memCount = sizeof(TraceInfo_Base);

    // We have 3 std::strings, so we need to account for their size
    if (eventName.size() > 0)
    {
        memCount += eventName.size() + 1; // +1 to account for null terminator
    }

    if (functionName.size() > 0)
    {
        memCount += functionName.size() + 1; // +1 to account for null terminator
    }

    if (fileName.size() > 0)
    {
        memCount += fileName.size() + 1; // +1 to account for null terminator
    }

    return memCount;
}

//-------------------------------------------------------------------------------------------------

std::string Babylon::Utils::TraceInfo_Mark::ToString() const
{
    std::stringstream out_str;

    out_str << "BabylonMarkEvent;" <<
                eventName << ";" <<
                GetTimeInNanoseconds(timestamp) << ";" <<
                eventTracedValue << ";" <<
                functionName << ";" <<
                fileName << ";" <<
                lineNumber;

    return out_str.str();
}

size_t Babylon::Utils::TraceInfo_Mark::GetMemoryFootprint() const
{
    return Babylon::Utils::TraceInfo_Base::GetMemoryFootprint() + sizeof(eventTracedValue);
}

//-------------------------------------------------------------------------------------------------

std::string Babylon::Utils::TraceInfo_Double::ToString() const
{
    std::stringstream out_str;

    out_str << "BabylonMeasurementEvent;" <<
                eventName << ";" <<
                GetTimeInNanoseconds(timestamp) << ";" <<
                eventTracedValue << ";" <<
                functionName << ";" <<
                fileName << ";" <<
                lineNumber;

    return out_str.str();
}

size_t Babylon::Utils::TraceInfo_Double::GetMemoryFootprint() const
{
    return Babylon::Utils::TraceInfo_Base::GetMemoryFootprint() + sizeof(eventTracedValue);
}

//-------------------------------------------------------------------------------------------------

std::string Babylon::Utils::TraceInfo_Char::ToString() const
{
    std::stringstream out_str;

    out_str << "BabylonMeasurementEvent;" <<
                eventName << ";" <<
                GetTimeInNanoseconds(timestamp) << ";" <<
                eventTracedValue << ";" <<
                functionName << ";" <<
                fileName << ";" <<
                lineNumber;

    return out_str.str();
}

size_t Babylon::Utils::TraceInfo_Char::GetMemoryFootprint() const
{
    return Babylon::Utils::TraceInfo_Base::GetMemoryFootprint() + (sizeof(std::string) + eventTracedValue.size());
}

//-------------------------------------------------------------------------------------------------

std::string Babylon::Utils::TraceInfo_ULongLong::ToString() const
{
    std::stringstream out_str;

    out_str << "BabylonMeasurementEvent;" <<
                eventName << ";" <<
                GetTimeInNanoseconds(timestamp) << ";" <<
                eventTracedValue << ";" <<
                functionName << ";" <<
                fileName << ";" <<
                lineNumber;

    return out_str.str();
}

size_t Babylon::Utils::TraceInfo_ULongLong::GetMemoryFootprint() const
{
    return Babylon::Utils::TraceInfo_Base::GetMemoryFootprint() + sizeof(eventTracedValue);
}

//-------------------------------------------------------------------------------------------------

std::string Babylon::Utils::TraceInfo_Timespan::ToString() const
{
    std::stringstream out_str;

    out_str << "BabylonTimeSpanEvent;" <<
                eventName << ";" <<
                GetTimeInNanoseconds(timestamp) << ";" <<
                GetTimeInNanoseconds(startTime) << ";" <<
                functionName << ";" <<
                fileName << ";" <<
                lineNumber;

    return out_str.str();
}

size_t Babylon::Utils::TraceInfo_Timespan::GetMemoryFootprint() const
{
    return Babylon::Utils::TraceInfo_Base::GetMemoryFootprint() + sizeof(startTime);
}

//-------------------------------------------------------------------------------------------------

// Magic number for back compat, prefer to use constructor with initial specification
static const int DEFAULT_TRACES_RESERVE = 250000;
Babylon::Utils::DefaultTraceProvider::DefaultTraceProvider()
    : DefaultTraceProvider(DEFAULT_TRACES_RESERVE)
{
}

Babylon::Utils::DefaultTraceProvider::DefaultTraceProvider(size_t InitialReserve) :
    m_Traces(),
    m_MemFootprint(sizeof(m_Traces)),
    InitialTraceReservation(InitialReserve)
{
}

Babylon::Utils::DefaultTraceProvider::~DefaultTraceProvider()
{
    m_Traces.clear();
}

void Babylon::Utils::DefaultTraceProvider::Register()
{
    // Preallocate our traces array so we don't re-alloc during perf testing
    m_Traces.reserve(InitialTraceReservation);
}

bool Babylon::Utils::DefaultTraceProvider::HasReallocTracesArray()
{
    return m_Traces.size() > InitialTraceReservation;
}

void Babylon::Utils::DefaultTraceProvider::TraceMark(
    const char* eventName,
    TimePoint   currentTime,
    int         eventTracedValue,
    const char* functionName,
    const char* fileName,
    int         lineNumber)
{
    std::unique_ptr<TraceInfo_Mark> traceinfo = std::make_unique<TraceInfo_Mark>();
    if (traceinfo != NULL)
    {
        traceinfo->eventName = eventName;
        traceinfo->timestamp = currentTime;
        traceinfo->functionName = functionName;
        traceinfo->fileName = fileName;
        traceinfo->lineNumber = lineNumber;
        traceinfo->eventTracedValue = eventTracedValue;

        // Increment our memory footprint tracker (size of pointer + memory footprint of the struct)
        m_MemFootprint += sizeof(std::unique_ptr<TraceInfo_Base>) + traceinfo->GetMemoryFootprint();

        m_Traces.push_back(std::move(traceinfo));
    }
}

void Babylon::Utils::DefaultTraceProvider::TraceMeasurement(
    const char* eventName,
    TimePoint   currentTime,
    double      eventTracedValue,
    const char* functionName,
    const char* fileName,
    int         lineNumber)
{
    std::unique_ptr<TraceInfo_Double> traceinfo = std::make_unique<TraceInfo_Double>();
    if (traceinfo != NULL)
    {
        traceinfo->eventName = eventName;
        traceinfo->timestamp = currentTime;
        traceinfo->functionName = functionName;
        traceinfo->fileName = fileName;
        traceinfo->lineNumber = lineNumber;
        traceinfo->eventTracedValue = eventTracedValue;

        // Increment our memory footprint tracker (size of pointer + memory footprint of the struct)
        m_MemFootprint += sizeof(std::unique_ptr<TraceInfo_Base>) + traceinfo->GetMemoryFootprint();

        m_Traces.push_back(std::move(traceinfo));
    }
}

void Babylon::Utils::DefaultTraceProvider::TraceMeasurement(
      const char* eventName,
      TimePoint   currentTime,
      unsigned long long eventTracedValue,
      const char* functionName,
      const char* fileName,
      int         lineNumber)
{
    std::unique_ptr<TraceInfo_ULongLong> traceinfo = std::make_unique<TraceInfo_ULongLong>();
    if (traceinfo != NULL)
    {
        traceinfo->eventName = eventName;
        traceinfo->timestamp = currentTime;
        traceinfo->functionName = functionName;
        traceinfo->fileName = fileName;
        traceinfo->lineNumber = lineNumber;
        traceinfo->eventTracedValue = eventTracedValue;

        // Increment our memory footprint tracker (size of pointer + memory footprint of the struct)
        m_MemFootprint += sizeof(std::unique_ptr<TraceInfo_Base>) + traceinfo->GetMemoryFootprint();

        m_Traces.push_back(std::move(traceinfo));
    }
}

void Babylon::Utils::DefaultTraceProvider::TraceMeasurement(
    const char* eventName,
    TimePoint   currentTime,
    const char* eventTracedValue,
    const char* functionName,
    const char* fileName,
    int         lineNumber)
{
    std::unique_ptr<TraceInfo_Char> traceinfo = std::make_unique<TraceInfo_Char>();
    if (traceinfo != NULL)
    {
        traceinfo->eventName = eventName;
        traceinfo->timestamp = currentTime;
        traceinfo->functionName = functionName;
        traceinfo->fileName = fileName;
        traceinfo->lineNumber = lineNumber;
        traceinfo->eventTracedValue = std::string(eventTracedValue);

        // Increment our memory footprint tracker (size of pointer + memory footprint of the struct)
        m_MemFootprint += sizeof(std::unique_ptr<TraceInfo_Base>) + traceinfo->GetMemoryFootprint();

        m_Traces.push_back(std::move(traceinfo));
    }
}

void Babylon::Utils::DefaultTraceProvider::TraceTimeSpan(
    const char* eventName,
    TimePoint   currentTime,
    TimePoint   startTime,
    const char* functionName,
    const char* fileName,
    int         lineNumber)
{
    std::unique_ptr<TraceInfo_Timespan> traceinfo = std::make_unique<TraceInfo_Timespan>();
    if (traceinfo != NULL)
    {
        traceinfo->eventName = eventName;
        traceinfo->timestamp = currentTime;
        traceinfo->functionName = functionName;
        traceinfo->fileName = fileName;
        traceinfo->lineNumber = lineNumber;
        traceinfo->startTime = startTime;

        // Increment our memory footprint tracker (size of pointer + memory footprint of the struct)
        m_MemFootprint += sizeof(std::unique_ptr<TraceInfo_Base>) + traceinfo->GetMemoryFootprint();

        m_Traces.push_back(std::move(traceinfo));
    }
}
