package com.example.bluetoothapp.domain

fun String.toBluetoothMessage() : BluetoothMessage {
    var sensorId = 0
    var data = 0
    val regex = Regex("""[0-9],[0-9]""")
    if (matches(regex)) {
        try {
            data = (substringBefore(",")).toInt()
            sensorId = (substringAfter(",")).toInt()
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