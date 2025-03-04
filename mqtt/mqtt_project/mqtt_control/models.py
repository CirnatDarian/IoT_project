from django.db import models

class TemperatureData(models.Model):
    value = models.FloatField()  # Po?i utiliza FloatField pentru valori numerice
    timestamp = models.DateTimeField(auto_now_add=True)

    def __str__(self):
        return f"Temp: {self.value} at {self.timestamp}"

class HumidityData(models.Model):
    value = models.FloatField()  # Folosim FloatField pentru a stoca umiditatea
    timestamp = models.DateTimeField(auto_now_add=True)

    def __str__(self):
        return f"Humidity: {self.value} at {self.timestamp}"

class RelayData(models.Model):
    state = models.CharField(max_length=255)
    timestamp = models.CharField(max_length=255)  

    def __str__(self):
        return f"Relay State: {self.state} at {self.timestamp}"
