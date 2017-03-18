from django.shortcuts import render
from .models import Temperature
from datetime import datetime, timedelta
from django.db.models import Avg

# Create your views here.
def index(request):
    latest = Temperature.objects.latest("date")
    ten_min_ago = datetime.now() - timedelta(minutes=10)
    one_hour_ago = datetime.now() - timedelta(hours=1)
    two_hour_ago = datetime.now() - timedelta(hours=2)
    thr_hour_ago = datetime.now() - timedelta(hours=3)
    fou_hour_ago = datetime.now() - timedelta(hours=4)
    
    ten_min = Temperature.objects.filter(date__gt=ten_min_ago).aggregate(Avg('air'), Avg("room"), Avg("probe_one"), Avg("probe_two"))
    one_hour = Temperature.objects.filter(date__gt=one_hour_ago).aggregate(Avg('air'), Avg("room"), Avg("probe_one"), Avg("probe_two"))
    two_hour = Temperature.objects.filter(date__gt=two_hour_ago, date__lt=one_hour_ago).aggregate(Avg('air'), Avg("room"), Avg("probe_one"), Avg("probe_two"))
    thr_hour = Temperature.objects.filter(date__gt=thr_hour_ago, date__lt=two_hour_ago).aggregate(Avg('air'), Avg("room"), Avg("probe_one"), Avg("probe_two"))
    fou_hour = Temperature.objects.filter(date__gt=fou_hour_ago, date__lt=thr_hour_ago).aggregate(Avg('air'), Avg("room"), Avg("probe_one"), Avg("probe_two"))
    return render(request, "index.html", {"last": latest, "ten_min": ten_min, "one_hour": one_hour, "two_hour": two_hour, "thr_hour": thr_hour, "fou_hour": fou_hour})
