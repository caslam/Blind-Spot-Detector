package com.example.bluetoothapp

import android.app.Activity
import com.example.bluetoothapp.ui.theme.BluetoothAppTheme
import android.bluetooth.BluetoothAdapter
import android.bluetooth.BluetoothDevice
import android.bluetooth.BluetoothManager
import android.bluetooth.BluetoothSocket
import android.content.Context
import android.content.pm.PackageManager
import android.hardware.Sensor
import android.hardware.SensorEvent
import android.hardware.SensorEventListener
import android.hardware.SensorManager
import android.os.Bundle
import android.os.Handler
import android.os.Looper
import android.os.Message
import android.util.Log
import android.widget.Toast
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.activity.enableEdgeToEdge
import androidx.compose.foundation.BorderStroke
import androidx.compose.foundation.Image
import androidx.compose.foundation.border
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.layout.width
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.AccountCircle
import androidx.compose.material3.Button
import androidx.compose.material3.Icon
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Surface
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.MutableState
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableIntStateOf
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.saveable.rememberSaveable
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.res.painterResource
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import androidx.core.app.ActivityCompat
import androidx.core.content.ContextCompat
import com.example.bluetoothapp.domain.BluetoothService
import com.example.bluetoothapp.domain.MESSAGE_READ
import com.example.bluetoothapp.domain.toBluetoothMessage
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import kotlinx.coroutines.withContext
import java.io.IOException
import java.nio.ByteBuffer
import java.nio.ByteOrder
import java.util.UUID

private val requiredPermissions = arrayOf(
    android.Manifest.permission.BLUETOOTH,
    android.Manifest.permission.BLUETOOTH_ADMIN,
//    android.Manifest.permission.BLUETOOTH_CONNECT,
//    android.Manifest.permission.BLUETOOTH_SCAN,
    android.Manifest.permission.ACCESS_FINE_LOCATION
)

const val REQUEST_CODE_PERMISSIONS = 0

class MainActivity : ComponentActivity(), SensorEventListener {
    private var bluetoothSocket: BluetoothSocket? = null
    private lateinit var bluetoothService: BluetoothService
    // Standard SerialPortService ID used by HC-06
    private val deviceUUID: UUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB")
    private lateinit var sensorManager: SensorManager
    private val gyroFrequency = 150L  // Transmission frequency of gyro data in ms
    private var gyroData: FloatArray = floatArrayOf(0f, 0f, 0f)

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        enableEdgeToEdge()
        setContent {
            BluetoothAppTheme {
                var count : MutableState<Int> = rememberSaveable { mutableIntStateOf(0) }
                var lidarData : MutableState<IntArray> = rememberSaveable { mutableStateOf(intArrayOf(0,0,0)) }
                var testArray : MutableState<IntArray> = rememberSaveable { mutableStateOf(intArrayOf(0,0,0)) }
                val handler = object : Handler(Looper.getMainLooper()) {
                    override fun handleMessage(msg: Message) {
                        when (msg.what) {
                            MESSAGE_READ -> {
                                val readBuffer = msg.obj as ByteArray
                                val readMessage = String(readBuffer, 0, msg.arg1 - 1).toBluetoothMessage()
                                if (readMessage.sensorId > 0) {
                                    if ((readMessage.sensorId == 4) && (readMessage.message == 1)) {
                                        Toast.makeText(this@MainActivity, "Pay attention to the road!", Toast.LENGTH_SHORT).show()
                                    } else if (readMessage.sensorId < 4) {
                                        println("${(readMessage.sensorId)}, ${(readMessage.message)}")
                                        Log.d("app_log","lidardata updated")
//                                        lidarData.value[readMessage.sensorId - 1] = readMessage.message
//                                        count.value += 1
                                        if (readMessage.sensorId - 1 == 0) {
                                            lidarData.value = intArrayOf(readMessage.message, lidarData.value[1], lidarData.value[2])
                                        } else if (readMessage.sensorId - 1 == 1) {
                                            lidarData.value = intArrayOf(lidarData.value[0], readMessage.message, lidarData.value[2])
                                        } else {
                                            lidarData.value = intArrayOf(lidarData.value[0], lidarData.value[1], readMessage.message)
                                        }

                                    }
                                }
                            }
                        }
                    }
                }
                val sendDataRunnable = object : Runnable {
                    override fun run() {
                        sendGyroData()
                        handler.postDelayed(this, gyroFrequency)
                    }
                }
                fun connectToDevice(device: BluetoothDevice) {
                    CoroutineScope(Dispatchers.IO).launch {
                        try {
                            bluetoothSocket = device.createRfcommSocketToServiceRecord(deviceUUID)
                            bluetoothSocket?.connect()
                            bluetoothService = BluetoothService(handler)
                            if (bluetoothSocket == null) {
                                println("BT socket was null")
                            } else {
                                bluetoothService.ConnectedThread(bluetoothSocket!!).start()
                                handler.post(sendDataRunnable)
                            }
                            withContext(Dispatchers.Main) {
                                Toast.makeText(this@MainActivity, "Connected to HC-06", Toast.LENGTH_SHORT).show()
                            }
                        } catch (e: IOException) {
                            e.printStackTrace()
                            withContext(Dispatchers.Main) {
                                Toast.makeText(this@MainActivity, "Connection failed", Toast.LENGTH_SHORT).show()
                            }
                        } catch (e: SecurityException) {
                            e.printStackTrace()
                        }
                    }
                }
                fun setupBluetooth() {
                    val bluetoothManager = getSystemService(Context.BLUETOOTH_SERVICE) as BluetoothManager
                    val bluetoothAdapter: BluetoothAdapter = bluetoothManager.adapter?: return

                    if (!bluetoothAdapter.isEnabled) {
                        return
                    }

                    try {
                        val pairedDevices: Set<BluetoothDevice>? = bluetoothAdapter.bondedDevices
                        pairedDevices?.forEach { device ->
                            // Attempt to connect to the HC-06
                            if (device.name == "HC-06") {
                                connectToDevice(device)
                                return@forEach
                            }
                        }
                    } catch (e: SecurityException) {
                        e.printStackTrace()
                    }
                }
                Surface(
                    modifier = Modifier.fillMaxSize(),
                    color = MaterialTheme.colorScheme.background
//                    color = Color.LightGray
                ) {

                    Row (horizontalArrangement = Arrangement.Center,
                        verticalAlignment = Alignment.Top,
                        modifier = Modifier.fillMaxWidth()
                            .padding(vertical = 64.dp)) {
                        Button (onClick = {
                            println("Connect clicked")
                            Log.d("app_log","connect clicked")
                            setupBluetooth()
                        },
                            modifier = Modifier.padding(16.dp, 16.dp)
                        ) {
                            Text("Connect")
                        }
                        Button (onClick =  {
                            println("Disconnect clicked")
                            Log.d("app_log","disconnect clicked")
                            handler.removeCallbacks(sendDataRunnable)
                            bluetoothSocket?.close()
                        },
                            modifier = Modifier.padding(16.dp, 16.dp)
                        ) {
                            Text("Disconnect")
                        }
                    }

                    LayoutScreen(lidarData, count)
                }
            }
        }

        if (!allPermissionsGranted()) {
            ActivityCompat.requestPermissions(this, requiredPermissions, REQUEST_CODE_PERMISSIONS)
        } else {
            // Permissions are already granted
            println("Perms granted")
        }
        setupGyro()
    }

    private fun allPermissionsGranted(): Boolean {
        return requiredPermissions.all { permission ->
            ContextCompat.checkSelfPermission(this, permission) == PackageManager.PERMISSION_GRANTED
        }
    }

    private fun FloatArray.toByteArray(): ByteArray {
        val buffer = ByteBuffer.allocate((this.size + 1) * 4) // 4 bytes per float
        var sum = 0f
        buffer.order(ByteOrder.LITTLE_ENDIAN)
        for (float in this) {
            buffer.putFloat(float)
        }
        for (float in this) {
            sum += float
        }
        buffer.putFloat(sum)
        return buffer.array()
    }

    private fun sendGyroData() {
        val bytes = gyroData.toByteArray()
//        println("${gyroData[0]}, ${gyroData[1]}, ${gyroData[2]}")
        bluetoothService.ConnectedThread(bluetoothSocket!!).write("$".toByteArray())
        bluetoothService.ConnectedThread(bluetoothSocket!!).write(bytes)
    }

    override fun onSensorChanged(event: SensorEvent?) {
        if (event?.sensor?.type == Sensor.TYPE_GYROSCOPE) {
            gyroData = event.values.clone()
        }
    }

    override fun onAccuracyChanged(sensor: Sensor?, accuracy: Int) {
        // No action taken
    }

    private fun setupGyro() {
        sensorManager = getSystemService(Context.SENSOR_SERVICE) as SensorManager
        val gyroSensor: Sensor? = sensorManager.getDefaultSensor(Sensor.TYPE_GYROSCOPE)
        if (gyroSensor != null) {
            gyroSensor.also { gyro ->
                sensorManager.registerListener(this, gyro, SensorManager.SENSOR_DELAY_UI)
            }
        } else {
            println("No gyroscope available")
        }
    }

    override fun onDestroy() {
        super.onDestroy()
        sensorManager.unregisterListener(this)
        bluetoothSocket?.close()
    }
}

@Composable
fun Person(username: String, modifier: Modifier = Modifier) {
    val imageModifier = Modifier
        .size(200.dp)
        .padding(20.dp)
//        .border(BorderStroke(1.dp, Color.Black))
    Row(horizontalArrangement = Arrangement.Center, verticalAlignment = Alignment.CenterVertically) {
        Column(
            modifier = imageModifier,
            verticalArrangement = Arrangement.Center,
            horizontalAlignment = Alignment.CenterHorizontally
        ) {
            if (username != "david") {
                Icon(Icons.Filled.AccountCircle, contentDescription = null)
            } else {
                Image(
                    painter = painterResource(id = R.drawable.squirrelicon2),
                    contentDescription = null,
//                    modifier = Modifier.background(Color.Black)
                )
            }
            if (username != "tyler") {
                Text("$username")
            } else {
                Text("lol")
            }
        }
    }
}

@Composable
fun LRZones(l_state : Int, r_state: Int,
          modifier: Modifier = Modifier) {
    Row (verticalAlignment = Alignment.Bottom, horizontalArrangement = Arrangement.Center) {
        val width = 200
        val height = 375
        Box() {
            val imageModifier = Modifier
                .width(width.dp)
                .height(height.dp)
//                .border(BorderStroke(1.dp, Color.Black))
            if (l_state == 0) {
                Image(
                    painter = painterResource(id = R.drawable.zone3),
                    modifier = imageModifier,
                    contentDescription = null
                )
                Log.d("app_log","zone3 drawn")
            } else {
                Image(
                    painter = painterResource(id = R.drawable.zone6),
                    modifier = imageModifier,
                    contentDescription = null
                )
                Log.d("app_log","zone6 drawn")
            }
        }
        Box() {
            val imageModifier = Modifier
                .width(width.dp)
                .height(height.dp)
//                .border(BorderStroke(1.dp, Color.Black))
//                .background(Color.Yellow)
            if (r_state == 0) {
                Image(
                    painter = painterResource(id = R.drawable.zone1),
                    modifier = imageModifier,
                    contentDescription = null
                )
                Log.d("app_log","zone1 drawn")
            } else {
                Image(
                    painter = painterResource(id = R.drawable.zone4),
                    modifier = imageModifier,
                    contentDescription = null
                )
                Log.d("app_log","zone4 drawn")
            }
        }
    }
}

@Composable
fun CZone(modifier: Modifier = Modifier, c_state : Int) {
    Column(verticalArrangement = Arrangement.Bottom, horizontalAlignment = Alignment.CenterHorizontally) {
        var count by remember { mutableStateOf(0) }
        val imageModifier = Modifier
            .size(340.dp)
            .border(BorderStroke(1.dp, Color.Black))
        if (c_state == 0) {
            Image(
                painter = painterResource(id = R.drawable.zone2),
                contentDescription = null,
                modifier = imageModifier
            )
            Log.d("app_log","zone2 drawn")
        } else {
            Image(
                painter = painterResource(id = R.drawable.zone5),
                contentDescription = null,
                modifier = imageModifier
            )
            Log.d("app_log","zone5 drawn")
        }

    }
}

@Composable
fun LayoutScreen(array: MutableState<IntArray>, count : MutableState<Int>, modifier: Modifier = Modifier) {
    if (count.value != -1) {
        LRZones(l_state = array.value[0], r_state = array.value[2])
        Person("User")
        CZone(c_state = array.value[1])
    }
}