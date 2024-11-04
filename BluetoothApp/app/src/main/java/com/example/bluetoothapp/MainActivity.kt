package com.example.bluetoothapp

import com.example.bluetoothapp.ui.theme.BluetoothAppTheme
import android.bluetooth.BluetoothAdapter
import android.bluetooth.BluetoothDevice
import android.bluetooth.BluetoothManager
import android.bluetooth.BluetoothSocket
import android.content.Context
import android.content.pm.PackageManager
import android.os.Bundle
import android.os.Handler
import android.os.Looper
import android.os.Message
import android.widget.Toast
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.activity.enableEdgeToEdge
import androidx.compose.foundation.Image
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
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.res.painterResource
import androidx.compose.ui.unit.dp
import androidx.core.app.ActivityCompat
import androidx.core.content.ContextCompat
import com.example.bluetoothapp.domain.BluetoothService
import com.example.bluetoothapp.domain.MESSAGE_READ
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import kotlinx.coroutines.withContext
import java.io.IOException
import java.util.UUID

private val requiredPermissions = arrayOf(
    android.Manifest.permission.BLUETOOTH,
    android.Manifest.permission.BLUETOOTH_ADMIN,
    android.Manifest.permission.BLUETOOTH_CONNECT,
    android.Manifest.permission.BLUETOOTH_SCAN,
    android.Manifest.permission.ACCESS_FINE_LOCATION
)

const val REQUEST_CODE_PERMISSIONS = 0

class MainActivity : ComponentActivity() {
    private var bluetoothSocket: BluetoothSocket? = null
    // Standard SerialPortService ID used by HC-06
    private val deviceUUID: UUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB")

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        enableEdgeToEdge()
        setContent {
            BluetoothAppTheme {
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
                            setupBluetooth()
                        },
                            modifier = Modifier.padding(16.dp, 16.dp)
                        ) {
                            Text("Connect")
                        }
                        Button (onClick =  {
                            println("Disconnect clicked")
                            bluetoothSocket?.close()
                        },
                            modifier = Modifier.padding(16.dp, 16.dp)
                        ) {
                            Text("Disconnect")
                        }
                    }
//                    Greeting("up")
                    Test1("down")
//                    TitleCardColumn()
//                    Person("david")
//                    Person("dat")
                    Person("cassandra")
//                    SimpleFilledTextFieldSample()
//                    Person("tyler")
                    Zones()
                }
            }
        }

        if (!allPermissionsGranted()) {
            ActivityCompat.requestPermissions(this, requiredPermissions, REQUEST_CODE_PERMISSIONS)
        } else {
            // Permissions are already granted
            println("Perms granted")
        }
    }

    private val handler = object : Handler(Looper.getMainLooper()) {
        override fun handleMessage(msg: Message) {
            when (msg.what) {
                MESSAGE_READ -> {
                    val readBuffer = msg.obj as ByteArray
                    val readMessage = String(readBuffer, 0, msg.arg1)
                    Toast.makeText(this@MainActivity, "Received: $readMessage", Toast.LENGTH_SHORT).show()
                }
//                MESSAGE_WRITE -> { }
//                MESSAGE_TOAST -> { }
            }
        }
    }

    private fun allPermissionsGranted(): Boolean {
        return requiredPermissions.all { permission ->
            ContextCompat.checkSelfPermission(this, permission) == PackageManager.PERMISSION_GRANTED
        }
    }

    private fun setupBluetooth() {
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

    private fun connectToDevice(device: BluetoothDevice) {
        CoroutineScope(Dispatchers.IO).launch {
            try {
                bluetoothSocket = device.createRfcommSocketToServiceRecord(deviceUUID)
                bluetoothSocket?.connect()
                val bluetoothService = BluetoothService(handler)
                if (bluetoothSocket == null) {
                    println("BT socket was null")
                } else {
                    bluetoothService.ConnectedThread(bluetoothSocket!!).start()
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

    override fun onDestroy() {
        super.onDestroy()
        bluetoothSocket?.close()
    }
}

@Composable
fun Person(username: String, modifier: Modifier = Modifier) {
    Row(horizontalArrangement = Arrangement.Center, verticalAlignment = Alignment.CenterVertically) {
        Column(
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
fun Test1(txt: String, modifier: Modifier = Modifier) {
    Row (verticalAlignment = Alignment.Bottom, horizontalArrangement = Arrangement.Center) {
        val width = 200
        val height = 375
        Box() {
            val imageModifier = Modifier
                .width(width.dp)
                .height(height.dp)
//                .border(BorderStroke(1.dp, Color.Black))
            Image(
                painter = painterResource(id = R.drawable.zone3),
                modifier = imageModifier,
                contentDescription = null
            )

        }
        Box() {
            val imageModifier = Modifier
                .width(width.dp)
                .height(height.dp)
//                .border(BorderStroke(1.dp, Color.Black))
//                .background(Color.Yellow)
            Image(
                painter = painterResource(id = R.drawable.zone1),
                modifier = imageModifier,
                contentDescription = null
            )

        }
    }
}

@Composable
fun Zones(modifier: Modifier = Modifier) {
//    Column (modifier = modifier.padding(56.dp),
//        horizontalAlignment = Alignment.CenterHorizontally,
//        verticalArrangement = Arrangement.Center) {
//        Row() {
//            Image(
//                painter = painterResource(id = R.drawable.zone1),
//                contentDescription = null)
////                contentScale = ContentScale.Fit)
//            Image(
//                painter = painterResource(id = R.drawable.zone1),
//                contentDescription = null)
////                contentScale = ContentScale.Fit)
//        }
//        Row {
//            Image(
//                painter = painterResource(id = R.drawable.zone1),
//                contentDescription = null)
////                contentScale = ContentScale.Fit)
//            Image(
//                painter = painterResource(id = R.drawable.zone1),
//                contentDescription = null)
//                contentScale = ContentScale.Fit)
//        }
//    }
    Column(verticalArrangement = Arrangement.Bottom, horizontalAlignment = Alignment.CenterHorizontally) {
        val imageModifier = Modifier
            .size(340.dp)
//                .border(BorderStroke(1.dp, Color.Black))
//                .background(Color.Yellow)
        Image(
            painter = painterResource(id = R.drawable.zone2),
            contentDescription = null,
            modifier = imageModifier
        )
    }
}