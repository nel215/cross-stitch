# coding: utf-8
import os
import pandas as pd
from datetime import datetime

data = []
for seed in os.listdir('./tmp'):
    with open('./tmp/' + seed) as f:
        logs = map(lambda s: s.strip(), f.readlines())

    info = {'seed': seed}
    for log in logs:
        if log.find('INFO:') == 0:
            d = log.split(' ')[1]
            k, v = d.split('=')
            info[k] = v
        if log.find('Score = ') == 0:
            s = "%.7f" % (float(log.split(' ')[2]))
            info['score'] = s

    data.append(info)

data.sort(key=lambda d: d['seed'])
df = pd.DataFrame(data).set_index('seed')
now = datetime.now()
date = now.strftime('%m%d')
os.makedirs('./report/%s' % (date), exist_ok=True)
to = 'report/%s/%s.csv' % (date, now.strftime('%H%M%S'))
print('dump to %s' % (to))
df.to_csv(to)
