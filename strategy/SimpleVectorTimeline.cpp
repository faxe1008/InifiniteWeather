#include "SimpleVectorTimeline.h"
#include <iostream>

static bool CompareTimelineEvents(
    const pool_unique_ptr<WeatherData::TimelineHeader> &first,
    const pool_unique_ptr<WeatherData::TimelineHeader> &second) {
  return first->timestamp > second->timestamp;
}

bool SimpleVectorTimeline::IsEventStillValid(
    const pool_unique_ptr<WeatherData::TimelineHeader> &uniquePtr,
    const uint64_t timestamp) {
  // Not getting timestamp in isEventStillValid for more speed
  return uniquePtr->timestamp + uniquePtr->expires >= timestamp;
}

uint64_t SimpleVectorTimeline::GetCurrentUnixTimestamp() const {
  return m_clock.get_current_time();
}

void SimpleVectorTimeline::TidyTimeline() {
  uint64_t timeCurrent = GetCurrentUnixTimestamp();

  m_events.erase(
      std::remove_if(
          std::begin(m_events), std::end(m_events),
          [&](pool_unique_ptr<WeatherData::TimelineHeader> const &header) {
            return !IsEventStillValid(header, timeCurrent);
          }),
      std::end(m_events));

  std::sort(std::begin(m_events), std::end(m_events), CompareTimelineEvents);
}

bool SimpleVectorTimeline::AddEventToTimeline(
    pool_unique_ptr<WeatherData::TimelineHeader> event) {

  if (m_events.size() == m_events.max_size()) {
    return false;
  }

  m_events.push_back(std::move(event));
  return true;
}

bool SimpleVectorTimeline::on_insert_event(const nlohmann::json &json_object) {

  int64_t current_time = m_clock.get_current_time();

  int64_t timestamp = json_object["Timestamp"];
  int64_t expires = json_object["Expires"];
  uint8_t type = json_object["EventType"];

  switch (static_cast<WeatherData::eventtype>(type)) {
  case WeatherData::eventtype::AirQuality: {
    auto airquality = pool_make_unique<WeatherData::AirQuality>();
    airquality->timestamp = timestamp;
    airquality->eventType = static_cast<WeatherData::eventtype>(type);
    airquality->expires = expires;

    // TODO: polluter
    std::string tmpPolluter = json_object["Polluter"];
    airquality->polluter = IString(tmpPolluter.c_str());

    airquality->amount = json_object["Amount"];
    if (!AddEventToTimeline(std::move(airquality))) {
      return false;
    }
    break;
  }
  case WeatherData::eventtype::Obscuration: {
    auto obscuration = pool_make_unique<WeatherData::Obscuration>();
    obscuration->timestamp = timestamp;
    obscuration->eventType = static_cast<WeatherData::eventtype>(type);
    obscuration->expires = expires;

    int64_t tmpType = json_object["Type"];
    obscuration->type = static_cast<WeatherData::obscurationtype>(tmpType);

    int64_t tmpAmount = json_object["Amount"];
    obscuration->amount = tmpAmount;

    if (!AddEventToTimeline(std::move(obscuration))) {
      return false;
    }
    break;
  }
  case WeatherData::eventtype::Precipitation: {
    auto precipitation = pool_make_unique<WeatherData::Precipitation>();
    precipitation->timestamp = timestamp;
    precipitation->eventType = static_cast<WeatherData::eventtype>(type);
    precipitation->expires = expires;

    int64_t tmpType = json_object["Type"];
    precipitation->type = static_cast<WeatherData::precipitationtype>(tmpType);

    int64_t tmpAmount = json_object["Amount"];
    precipitation->amount = tmpAmount;

    if (!AddEventToTimeline(std::move(precipitation))) {
      return false;
    }
    break;
  }
  case WeatherData::eventtype::Wind: {
    auto wind = pool_make_unique<WeatherData::Wind>();
    wind->timestamp = timestamp;
    wind->eventType = static_cast<WeatherData::eventtype>(type);
    wind->expires = expires;

    int64_t tmpMin = json_object["SpeedMin"];
    wind->speedMin = tmpMin;

    int64_t tmpMax = json_object["SpeedMax"];
    wind->speedMax = tmpMax;

    int64_t tmpDMin = json_object["DirectionMin"];
    wind->directionMin = tmpDMin;

    int64_t tmpDMax = json_object["DirectionMax"];
    wind->directionMax = tmpDMax;

    if (!AddEventToTimeline(std::move(wind))) {
      return false;
    }
    break;
  }
  case WeatherData::eventtype::Temperature: {
    auto temperature = pool_make_unique<WeatherData::Temperature>();
    temperature->timestamp = timestamp;
    temperature->eventType = static_cast<WeatherData::eventtype>(type);
    temperature->expires = expires;

    int64_t tmpTemperature = json_object["Temperature"];
    temperature->temperature = static_cast<int16_t>(tmpTemperature);

    int64_t tmpDewPoint = json_object["DewPoint"];
    temperature->dewPoint = static_cast<int16_t>(tmpDewPoint);

    return AddEventToTimeline(std::move(temperature));
  }
  case WeatherData::eventtype::Special: {
    auto special = pool_make_unique<WeatherData::Special>();
    special->timestamp = timestamp;
    special->eventType = static_cast<WeatherData::eventtype>(type);
    special->expires = expires;

    int64_t tmpType = json_object["Type"];
    special->type = static_cast<WeatherData::specialtype>(tmpType);

    if (!AddEventToTimeline(std::move(special))) {
      return false;
    }
    break;
  }
  case WeatherData::eventtype::Pressure: {
    auto pressure = pool_make_unique<WeatherData::Pressure>();
    pressure->timestamp = timestamp;
    pressure->eventType = static_cast<WeatherData::eventtype>(type);
    pressure->expires = expires;

    int64_t tmpPressure = json_object["Pressure"];
    pressure->pressure = tmpPressure;
    if (!AddEventToTimeline(std::move(pressure))) {
      return false;
    }
    break;
  }
  case WeatherData::eventtype::Location: {
    auto location = pool_make_unique<WeatherData::Location>();
    location->timestamp = timestamp;
    location->eventType = static_cast<WeatherData::eventtype>(type);
    location->expires = expires;

    std::string tmpLocation = json_object["Location"];
    location->location = IString(tmpLocation.c_str());

    if (!AddEventToTimeline(std::move(location))) {
      return false;
    }
    break;
  }
  case WeatherData::eventtype::Clouds: {
    auto clouds = pool_make_unique<WeatherData::Clouds>();
    clouds->timestamp = timestamp;
    clouds->eventType = static_cast<WeatherData::eventtype>(type);
    clouds->expires = expires;

    int64_t tmpAmount = json_object["Amount"];
    clouds->amount = static_cast<uint8_t>(tmpAmount);

    if (!AddEventToTimeline(std::move(clouds))) {
      return false;
    }
    break;
  }
  case WeatherData::eventtype::Humidity: {
    auto humidity = pool_make_unique<WeatherData::Humidity>();
    humidity->timestamp = timestamp;
    humidity->eventType = static_cast<WeatherData::eventtype>(type);
    humidity->expires = expires;

    int64_t tmpType = json_object["Humidity"];
    humidity->humidity = static_cast<uint8_t>(tmpType);

    if (!AddEventToTimeline(std::move(humidity))) {
      return false;
    }
    break;
  }
  default: {
    return false;
  }
  }

  TidyTimeline();

  return true;
}