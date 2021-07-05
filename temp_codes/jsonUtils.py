import json
import csv

f = open('graphdata.json')
data = json.load(f)
data = data['links']
data = [['source', 'target', 'state', 'type']]+[[d['source'], d['target'], d['state'], d['type']] for d in data]
with open("transmissions.csv", "w", newline="") as f:
    writer = csv.writer(f)
    writer.writerows(data)

