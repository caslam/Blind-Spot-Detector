package com.example.bluetoothapp.domain

fun parseMessages(msgs: String) : List<String> {
    return msgs.split("\n")
}

fun String.toBluetoothMessage() : BluetoothMessage {
    var sensorId = ""
    var data = ""
    val regex = Regex(".*_.*#")
    if (matches(regex)) {
        val message = substringBefore("#")
        sensorId = message.substringBefore("_")
        data = message.substringAfter("_")
    }

    return BluetoothMessage(sensorId = sensorId, message = data)
}

data class BluetoothMessage (
    val sensorId: String,
    val message: String
)