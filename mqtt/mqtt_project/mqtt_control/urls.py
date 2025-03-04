from django.urls import path
from . import views

urlpatterns = [
    path('control_relay/', views.control_relay, name='control_relay'),
    path('', views.index, name='index'),
]
