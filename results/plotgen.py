import numpy as np
import matplotlib.pyplot as plt

def calculate_ema_manual(data, window):
    ema = []
    smoothing_factor = 2 / (window + 1)
    ema.append(data[0])  # EMA of the first value is just the first value itself
    for i in range(1, len(data)):
        ema_value = (data[i] - ema[-1]) * smoothing_factor + ema[-1]
        ema.append(ema_value)
    return ema

def smooth_moving_average(arr, window):
    result = []
    for i in range(len(arr)):
        start_index = max(0, i - window + 1)
        end_index = i + 1
        window_data = arr[start_index:end_index]
        average = sum(window_data) / len(window_data)
        result.append(average)
    return result

def cumulative_sum(arr):
    result = []
    cumulative_sum = 0
    for num in arr:
        cumulative_sum += num
        result.append(cumulative_sum)
    return result

t1_w1={'est': [], 'ref': []}
t1_w1_mov={'est': [], 'ref': []}
t1_wpl={'est': [], 'ref': []}
t1_wpl_mov={'est': [], 'ref': []}

t2_w1={'est': [], 'ref': []}
t2_w1_mov={'est': [], 'ref': []}
t2_wpl={'est': [], 'ref': []}
t2_wpl_mov={'est': [], 'ref': []}

file_list_w1=['test1-wss1_1_20.txt', 'test1-wss1-moving.txt', 'test2-wss1_15_20.txt', 'test2-wss1-moving.txt']
file_list_wpl=['test1-wsspl_1_20.txt', 'test1-wsspl-moving.txt', 'test2-wsspl_15_20.txt', 'test2-wsspl-moving.txt']

for filename in file_list_w1:
    with open(filename, 'r') as file:
        next(file)
        if filename=='test1-wss1_1_20.txt' or filename=='test2-wss1_15_20.txt':
            next(file)
        for line in file:
            columns = line.strip().split()
            if columns:
                if filename=='test1-wss1_1_20.txt':
                    t1_w1['est'].append(float(columns[0]))
                    t1_w1['ref'].append(float(columns[1]))
                elif filename=='test1-wss1-moving.txt':
                    t1_w1_mov['est'].append(float(columns[0]))
                    t1_w1_mov['ref'].append(float(columns[1]))
                elif filename=='test2-wss1_15_20.txt':
                    t2_w1['est'].append(float(columns[0]))
                    t2_w1['ref'].append(float(columns[1]))
                elif filename=='test2-wss1-moving.txt':
                    t2_w1_mov['est'].append(float(columns[0]))
                    t2_w1_mov['ref'].append(float(columns[1]))

for filename in file_list_wpl:
    with open(filename, 'r') as file:
        next(file)
        if filename=='test1-wsspl_1_20.txt' or filename=='test2-wsspl_15_20.txt':
            next(file)
        for line in file:
            columns = line.strip().split()
            if columns:
                if filename=='test1-wsspl_1_20.txt':
                    t1_wpl['est'].append(float(columns[0]))
                    t1_wpl['ref'].append(float(columns[3]))
                elif filename=='test1-wsspl-moving.txt':
                    t1_wpl_mov['est'].append(float(columns[0]))
                    t1_wpl_mov['ref'].append(float(columns[3]))
                elif filename=='test2-wsspl_15_20.txt':
                    t2_wpl['est'].append(float(columns[0]))
                    t2_wpl['ref'].append(float(columns[3]))
                elif filename=='test2-wsspl-moving.txt':
                    t2_wpl_mov['est'].append(float(columns[0]))
                    t2_wpl_mov['ref'].append(float(columns[3]))

# print(t2_wpl)
# exit()                    
#Plot figure with t1_w1 and t1_wpl
plt.plot(t1_w1['est'], t1_w1['ref'], 'ro', label='Idle bit', linestyle='dashed')
plt.plot(t1_wpl['est'], t1_wpl['ref'], 'bo', label='Acess bit', linestyle='dashed')
plt.xlabel('Estimated Time Duration (s)')
plt.ylabel('Total memory referenced (KB)')
plt.title('Test 1 - Cumulative Working Set Size')
plt.legend()
plt.savefig('test1_1.png')

#Plot figure with t2_w1 and t2_wpl
#Get new figure
plt.figure()
plt.plot(t2_w1['est'][2:], t2_w1['ref'][2:], 'ro', label='Idle bit', linestyle='dashed')
plt.plot(t2_wpl['est'][2:], t2_wpl['ref'][2:], 'bo', label='Acess bit', linestyle='dashed')
plt.xlabel('Estimated Time Duration (s)')
plt.ylabel('Total memory referenced (KB)')
plt.title('Test 2 - Cumulative Working Set Size')
plt.legend()
plt.savefig('test2_1.png')

#Plot figure with t1_w1_mov and t1_wpl_mov
#Calculate exponential moving average of t1_w1_mov and t1_wpl_mov
# t1_w1_mov['ema'] = calculate_ema_manual(t1_w1_mov['ref'], 10)
# t1_wpl_mov['ema'] = calculate_ema_manual(t1_wpl_mov['ref'], 10)
t1_w1_mov['ema'] = smooth_moving_average(t1_w1_mov['ref'], 10)
t1_wpl_mov['ema'] = smooth_moving_average(t1_wpl_mov['ref'], 10)
plt.figure()
plt.plot(cumulative_sum(t1_w1_mov['est']), t1_w1_mov['ema'], 'ro', label='Idle bit', linestyle='dashed')
plt.plot(cumulative_sum(t1_wpl_mov['est']), t1_wpl_mov['ema'], 'bo', label='Acess bit', linestyle='dashed')
plt.xlabel('Estimated Time Duration (s)')
plt.ylabel('Working Set size (KB)')
plt.title('Test 1 - WSS (Moving Average over Time Intervals)')
plt.legend()
plt.savefig('test1_2.png')

#Plot figure with t2_w1_mov and t2_wpl_mov
#Calculate exponential moving average of t2_w1_mov and t2_wpl_mov
# t2_w1_mov['ema'] = calculate_ema_manual(t2_w1_mov['ref'], 10)
# t2_wpl_mov['ema'] = calculate_ema_manual(t2_wpl_mov['ref'], 10)
t2_w1_mov['ema'] = smooth_moving_average(t2_w1_mov['ref'], 10)
t2_wpl_mov['ema'] = smooth_moving_average(t2_wpl_mov['ref'], 10)
plt.figure()
plt.plot(cumulative_sum(t2_w1_mov['est']), t2_w1_mov['ema'], 'ro', label='Idle bit', linestyle='dashed')
plt.plot(cumulative_sum(t2_wpl_mov['est']), t2_wpl_mov['ema'], 'bo', label='Acess bit', linestyle='dashed')
plt.xlabel('Estimated Time Duration (s)')
plt.ylabel('Working Set size (KB)')
plt.title('Test 2 - WSS (Moving Average over Time Intervals)')
plt.legend()
plt.savefig('test2_2.png')
