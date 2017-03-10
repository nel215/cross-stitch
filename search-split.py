# coding: utf-8
import os
import pandas as pd

data = []
for seed in os.listdir('./tmp'):
    with open('./tmp/' + seed) as f:
        logs = map(lambda s: s.strip(), f.readlines())

    for log in logs:
        info = {}
        if log.find('DEBUG: L=') == 0:
            f = lambda s: s.split('=')[1]
            L, SS, RS = map(f, log.split(' ')[1].split(',')) 
            info['L'] = L
            info['SS'] = SS
            info['RS'] = RS
            data.append(info)


df = pd.DataFrame(data)
df.to_csv('split.csv')
