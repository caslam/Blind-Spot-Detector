package com.example.bluetoothapp.domain

fun String.toBluetoothMessage() : BluetoothMessage {
    var sensorId = 0
    var data = 0
    val message = substringBefore("#")
    data = (message.substringBefore(",")).toInt()
    sensorId = (message.substringAfter(",")).toInt()

    return BluetoothMessage(sensorId = sensorId, message = data)
}

data class BluetoothMessage (
    val sensorId: Int,
    val message: Int
)