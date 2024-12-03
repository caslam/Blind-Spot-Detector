package com.example.bluetoothapp.domain

fun String.toBluetoothMessage() : BluetoothMessage {
    var sensorId = 0
    var data = 0
    val regex = Regex("""\$[0-1],[1-4]""")
    if (matches(regex)) {
        try {
            val message = substringAfter("$")
            data = (message.substringBefore(",")).toInt()
            sensorId = (message.substringAfter(",")).toInt()
        } catch (e: NumberFormatException) {
            return BluetoothMessage(sensorId = 0, message = 0)
        }
    }
    return BluetoothMessage(sensorId = sensorId, message = data)
}

data class BluetoothMessage (
    val sensorId: Int,
    val message: Int
)