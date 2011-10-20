#!/usr/bin/env python

import sys
import os
import sqlite3

H_DIR = '/home/victor/off2/'

BaseName = 'errdb'

print 'Create DB at location:', H_DIR+BaseName

conn = sqlite3.connect(H_DIR+BaseName)

print 'Create tables differences at DB'

c = conn.cursor()
c.execute('''create table differences
( ID INTEGER primery key,
  Line0 INTEGER,
  Line1 INTEGER,
  Start0 INTEGER,
  Start1 INTEGER,
  End0 INTEGER,
  End1 INTEGER,
  Text0 TEXT,
  Text1 TEXT)''')


conn.commit()

c.close()

