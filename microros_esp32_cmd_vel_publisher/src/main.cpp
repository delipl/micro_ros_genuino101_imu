#include <Arduino.h>
#include <SoftwareSerial.h>
#include <geometry_msgs/msg/twist.h>
#include <micro_ros_platformio.h>
#include <rcl/error_handling.h>
#include <rcl/rcl.h>
#include <rclc/executor.h>
#include <rclc/rclc.h>
#include <stdio.h>

// As both during TX and RX softSerial is disabling interrupts
// you can not send data from RX to TX on a same ESP32
// left the original example in just to show how to use

// RX = pin 14, TX = 12, none-invert, buffersize 256.
SoftwareSerial swSer(14, 12, false, 256);

rcl_publisher_t publisher;
geometry_msgs__msg__Twist msg;
rclc_support_t support;
rcl_allocator_t allocator;
rcl_node_t node;

#define LED_PIN 13

#define RCCHECK(fn)                    \
    {                                  \
        rcl_ret_t temp_rc = fn;        \
        if ((temp_rc != RCL_RET_OK)) { \
            error_loop();              \
        }                              \
    }
#define RCSOFTCHECK(fn)                \
    {                                  \
        rcl_ret_t temp_rc = fn;        \
        if ((temp_rc != RCL_RET_OK)) { \
        }                              \
    }

void error_loop() {
    while (1) {
        digitalWrite(LED_PIN, !digitalRead(LED_PIN));
        delay(100);
    }
}


void setup() {
    IPAddress ip(192, 168, 2, 153);
    set_microros_wifi_transports("KoNaR_T3", "konarpany_down", ip, 8888);
    swSer.begin(9600);
    Serial.begin(9600);

    delay(2000);

    allocator = rcl_get_default_allocator();

    // create init_options
    RCCHECK(rclc_support_init(&support, 0, NULL, &allocator));

    // create node
    RCCHECK(rclc_node_init_default(&node, "micro_ros_rp2040_wifi_node", "", &support));

    // create publisher
    RCCHECK(rclc_publisher_init_default(
        &publisher,
        &node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(geometry_msgs, msg, Twist),
        "cmd_vel"));
    msg.linear.x = 0.0;
}

void loop() {
    String ready = swSer.readStringUntil('\n');
    float x, z;
    sscanf(ready.c_str(), "%f%f", &x, &z);
    Serial.println(ready);
    msg.linear.x = z*0.5;
    msg.angular.z = x*0.5;

    RCSOFTCHECK(rcl_publish(&publisher, &msg, NULL));
    delay(100);
    // delay(100);
}