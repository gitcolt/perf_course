#!/usr/bin/env python3

from math import radians, sin, cos, sqrt, asin
import time 
import json 

json_file = open('out.json')

start_time = time.time()
json_input = json.load(json_file);
mid_time = time.time()

def haversine_of_degrees(x0, y0, x1, y1, r):
    dy = radians(y1 - y0)
    dx = radians(x1 - x0)
    y0 = radians(y0)
    y1 = radians(y1)

    root_term = (sin(dy/2)**2) + cos(y0)*cos(y1)*(sin(dx/2)**2)
    res = 2*r*asin(sqrt(root_term))

    return res

EARTH_RADIUS_KM = 6371

sum = 0
count = 0
for pair in json_input['pairs']:
    sum += haversine_of_degrees(pair['x0'], pair['y0'], pair['x1'], pair['y1'], EARTH_RADIUS_KM)
    count += 1
average = sum / count
end_time = time.time()

print(f'result: {str(average)}')
print(f'input: {str(mid_time - start_time)} seconds')
print(f'math: {str(end_time - mid_time)} seconds')
print(f'total: {str(end_time - start_time)} seconds')
print(f'throughput: {str(count / (end_time - start_time))} haversines/second')
