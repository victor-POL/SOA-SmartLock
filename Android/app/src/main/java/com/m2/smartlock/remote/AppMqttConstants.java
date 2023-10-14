package com.m2.smartlock.remote;

public class AppMqttConstants {
    public static final String TOPIC_APP_COMMAND = "app-command";
    public static final String TOPIC_VALUE_APP_COMMAND_UNLOCK = "unlock";

    public static final String TOPIC_APP_SET_PASS = "app-setpass";

    public static final String TOPIC_ESP_DOOR_STATUS = "esp-doorstatus";
    public static final String TOPIC_VALUE_ESP_DOOR_STATUS_OPEN = "open";
    public static final String TOPIC_VALUE_ESP_DOOR_STATUS_CLOSED = "closed";

    public static final String TOPIC_ESP_UNLOCK = "esp-unlock";
    public static final String TOPIC_VALUE_ESP_UNLOCK_SUCCESS = "success";
    public static final String TOPIC_VALUE_ESP_UNLOCK_ERROR = "error";
    
    public static final String BROKER_URL = "tcp://broker.emqx.io:1883";
    public static final String CLIENT_ID = "";
    public static final String USER = "";
    public static final String PASS = "";
}
