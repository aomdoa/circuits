from django.db import models

# Create your models here.
class Temperature(models.Model):
    date = models.DateTimeField(db_column="sample_date")
    sample = models.IntegerField()
    air = models.FloatField(db_column="sensor_3")
    room = models.FloatField(db_column="sensor_4")
    probe_one = models.FloatField(db_column="sensor_1")
    probe_two = models.FloatField(db_column="sensor_2")
    
    class Meta:
        db_table = "temperatures"