# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
    ]

    operations = [
        migrations.CreateModel(
            name='Temperature',
            fields=[
                ('id', models.AutoField(primary_key=True, verbose_name='ID', auto_created=True, serialize=False)),
                ('date', models.DateTimeField(db_column='sample_date')),
                ('sample', models.IntegerField()),
                ('air', models.FloatField(db_column='sensor_3')),
                ('room', models.FloatField(db_column='sensor_4')),
                ('probe_one', models.FloatField(db_column='sensor_1')),
                ('probe_two', models.FloatField(db_column='sensor_2')),
            ],
        ),
    ]
