
import pandas as pd
import random
import numpy as np
from datetime import datetime, timedelta

# Function to generate random timestamps
def generate_grouped_timestamps(start, end, total_days, daily_events):
    start_time = datetime.strptime(start, "%Y-%m-%d %H:%M:%S")
    end_time = datetime.strptime(end, "%Y-%m-%d %H:%M:%S")
    delta_per_day = timedelta(days=(end_time - start_time).days / total_days)

    timestamps = []
    current_day = start_time
    for _ in range(total_days):
        for _ in range(daily_events):
            timestamps.append(
                current_day + timedelta(seconds=random.randint(0, 86400 - 1))
            )
        current_day += delta_per_day

    return sorted(timestamps)

# Parameters
num_days = 100  # Number of days to simulate
daily_events = 100  # Events per day
start_date = "2023-07-01 00:00:00"
end_date = "2024-10-01 00:00:00"

# Generate synthetic data
timestamps = generate_grouped_timestamps(start_date, end_date, num_days, daily_events)
proximity_distances = [random.randint(1, 100) for _ in range(num_days * daily_events)]
force_detected = [random.choice([0, 1]) for _ in range(num_days * daily_events)]
intruder_detected = [random.choice([0, 1]) for _ in range(num_days * daily_events)]

# Logic for soft and hard alarms
soft_alarm_triggered = [
    1 if proximity_distances[i] < 50 and intruder_detected[i] == 1 else 0
    for i in range(num_days * daily_events)
]
hard_alarm_triggered = [
    1 if force_detected[i] == 1 else 0
    for i in range(num_days * daily_events)
]

# Notification type
notification_types = [
    "Proximity Warning" if soft_alarm_triggered[i] == 1 and hard_alarm_triggered[i] == 0
    else "Force Alert" if hard_alarm_triggered[i] == 1
    else None
    for i in range(num_days * daily_events)
]

# Notification sent
notifications_sent = [1 if notification_types[i] else 0 for i in range(num_days * daily_events)]

# Alarm reset times (only for hard alarms)
alarm_reset_times = [
    random.randint(5, 20) if hard_alarm_triggered[i] == 1 else 0 for i in range(num_days * daily_events)
]

# Event type
event_types = [
    "Proximity Detection" if soft_alarm_triggered[i] == 1 and hard_alarm_triggered[i] == 0
    else "Force Detection" if hard_alarm_triggered[i] == 1
    else "No Event"
    for i in range(num_days * daily_events)
]

# System uptime (cumulative random increments of 1-10 seconds)
system_uptime = np.cumsum(np.random.randint(1, 10, num_days * daily_events))

# Create DataFrame
data = pd.DataFrame({
    "Timestamp": timestamps,
    "Proximity Distance (cm)": proximity_distances,
    "Intruder Detected": intruder_detected,
    "Force Detected": force_detected,
    "Soft Alarm Triggered": soft_alarm_triggered,
    "Hard Alarm Triggered": hard_alarm_triggered,
    "Notification Type": notification_types,
    "Notification Sent": notifications_sent,
    "Alarm Reset Time (s)": alarm_reset_times,
    "Event Type": event_types,
    "System Uptime (s)": system_uptime,
})

# Save to CSV
data.to_csv("synthetic_intrusion_data.csv", index=False)

print("Synthetic dataset with updated logic generated and saved as 'synthetic_intrusion_data.csv'.")


# import pandas as pd
# import random
# import numpy as np
# from datetime import datetime, timedelta

# # Function to generate random timestamps
# def generate_grouped_timestamps(start, end, total_days, daily_events):
#     start_time = datetime.strptime(start, "%Y-%m-%d %H:%M:%S")
#     end_time = datetime.strptime(end, "%Y-%m-%d %H:%M:%S")
#     delta_per_day = timedelta(days=(end_time - start_time).days / total_days)

#     timestamps = []
#     current_day = start_time
#     for _ in range(total_days):
#         for _ in range(daily_events):
#             timestamps.append(
#                 current_day + timedelta(seconds=random.randint(0, 86400 - 1))
#             )
#         current_day += delta_per_day

#     return sorted(timestamps)

# # Parameters
# num_days = 100  # Number of days to simulate
# daily_events = 100  # Events per day
# start_date = "2023-07-01 00:00:00"
# end_date = "2024-10-01 00:00:00"

# # Generate synthetic data
# timestamps = generate_grouped_timestamps(start_date, end_date, num_days, daily_events)
# proximity_distances = [random.randint(1, 100) for _ in range(num_days * daily_events)]

# # Logic to limit intrusion detection to 5-6 times per month
# intruder_detected = [0] * (num_days * daily_events)
# for day in range(num_days):
#     monthly_events = 6  # Max 6 intrusion detections per month
#     start_index = day * daily_events
#     end_index = start_index + daily_events
#     intrusion_indices = random.sample(range(start_index, end_index), k=monthly_events)
#     for idx in intrusion_indices:
#         intruder_detected[idx] = 1

# # Logic to limit force detection to 1-2 times per month
# force_detected = [0] * (num_days * daily_events)
# for day in range(num_days):
#     monthly_force_events = 2  # Max 2 force detections per month
#     start_index = day * daily_events
#     end_index = start_index + daily_events
#     force_indices = random.sample(range(start_index, end_index), k=monthly_force_events)
#     for idx in force_indices:
#         force_detected[idx] = 1

# # Logic for soft and hard alarms
# soft_alarm_triggered = [
#     1 if proximity_distances[i] < 50 and intruder_detected[i] == 1 else 0
#     for i in range(num_days * daily_events)
# ]
# hard_alarm_triggered = [
#     1 if force_detected[i] == 1 and proximity_distances[i] < 50 and intruder_detected[i] == 1 else 0
#     for i in range(num_days * daily_events)
# ]

# # Notification type
# notification_types = [
#     "Proximity Warning" if soft_alarm_triggered[i] == 1 and hard_alarm_triggered[i] == 0
#     else "Force Alert" if hard_alarm_triggered[i] == 1
#     else None
#     for i in range(num_days * daily_events)
# ]

# # Notification sent
# notifications_sent = [1 if notification_types[i] else 0 for i in range(num_days * daily_events)]

# # Alarm reset times (only for hard alarms)
# alarm_reset_times = [
#     random.randint(5, 20) if hard_alarm_triggered[i] == 1 else 0 for i in range(num_days * daily_events)
# ]

# # Event type
# event_types = [
#     "Proximity Detection" if soft_alarm_triggered[i] == 1 and hard_alarm_triggered[i] == 0
#     else "Force Detection" if hard_alarm_triggered[i] == 1
#     else "No Event"
#     for i in range(num_days * daily_events)
# ]

# # System uptime (cumulative random increments of 1-10 seconds)
# system_uptime = np.cumsum(np.random.randint(1, 10, num_days * daily_events))

# # Create DataFrame
# data = pd.DataFrame({
#     "Timestamp": timestamps,
#     "Proximity Distance (cm)": proximity_distances,
#     "Intruder Detected": intruder_detected,
#     "Force Detected": force_detected,
#     "Soft Alarm Triggered": soft_alarm_triggered,
#     "Hard Alarm Triggered": hard_alarm_triggered,
#     "Notification Type": notification_types,
#     "Notification Sent": notifications_sent,
#     "Alarm Reset Time (s)": alarm_reset_times,
#     "Event Type": event_types,
#     "System Uptime (s)": system_uptime,
# })

# # Save to CSV
# data.to_csv("synthetic_intrusion_data.csv", index=False)

# print("Synthetic dataset with updated logic generated and saved as 'synthetic_intrusion_data.csv'.")
