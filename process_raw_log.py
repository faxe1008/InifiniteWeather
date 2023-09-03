import sys
from datetime import datetime
import json
import re


def get_date(file_name, hour, minute, second):
    result = re.search(r".*?gadgetbridge-(.*?)\.0\.log", file_name)
    dt = datetime.strptime(result.group(1), "%Y-%m-%d")
    kwargs = {}
    kwargs['hour']  = hour
    kwargs['minute'] = minute
    kwargs['second'] = second
    dt = dt.replace(**kwargs)
    return dt


result = []


event_type_map = ["Obscuration", "Precipitation", "Wind", "Temperature", "AirQuality", "Special", "Pressure", "Location", "Clouds", "Humidity"]

with open(sys.argv[1], "r") as f:
    for line in f:
        if not "WEATHER_LOG" in line:
            continue
        parts = line.split(" ")

        # Get the event clock
        clock_raw_str = parts[0]
        event_dt = datetime.strptime(clock_raw_str, '%H:%M:%S.%f')
        event_clock = get_date(sys.argv[1], event_dt.hour, event_dt.minute, event_dt.second)
        
        # Get the json
        event_data = "".join(parts[7:]).strip()
        # Convert to Int
        event_json = json.loads(event_data)
        event_json["EventType"] = event_type_map.index(event_json["EventType"]);

        result.append({
            "clock": int(event_clock.timestamp()),
            "data" : event_json
        })


with open(sys.argv[1] + "_cleaned", "w") as f:
    json.dump(result, f, indent=4)
