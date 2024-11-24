package com.example.scoreboard2

import android.Manifest
import android.bluetooth.BluetoothAdapter
import android.bluetooth.BluetoothDevice
import android.bluetooth.BluetoothGatt
import android.bluetooth.BluetoothGattCallback
import android.bluetooth.BluetoothGattCharacteristic
import android.content.pm.PackageManager
import android.os.Bundle
import android.util.Log
import android.widget.Button
import android.widget.ImageButton
import androidx.appcompat.app.AppCompatActivity
import androidx.core.app.ActivityCompat
import java.util.UUID
import android.widget.EditText

// List of commands
const val CMD_SIDE_A_SET =     5
const val CMD_SIDE_B_SET =     6
const val CMD_RESET_COUNTERS = 10
const val CMD_DISPLAY_ON_OFF = 11
const val CMD_CHANGE_SIDES =   12

// ScoreBoard BLE values
//const val SCOREBOARD_MAC = "C0:C3:45:37:24:51"
const val SCOREBOARD_MAC = "C0:C3:11:2B:A9:51"
const val SCOREBOARD_SERVICE = "45340637-DEA7-48D7-9262-5F392E4317C6"
const val SCOREBOARD_COMMAND_CHARACTERISTIC = "81044b7b-7fb3-41e1-9127-a8730afd24ff"

class MainActivity : AppCompatActivity() {
    private val REQUEST_BLUETOOTH_CONNECT = 1
    private lateinit var bluetoothAdapter: BluetoothAdapter
    private lateinit var bluetoothDevice: BluetoothDevice
    private lateinit var bluetoothGatt: BluetoothGatt
    lateinit var bluetoothCharacteristicCommand: BluetoothGattCharacteristic

    private val scoreService = SCOREBOARD_SERVICE
    private val commandCharacteristic = SCOREBOARD_COMMAND_CHARACTERISTIC

    override fun onPause() {
        super.onPause()
        // Perform actions when the app loses focus (goes into the background)
        Log.d("MainActivity", "App is losing focus")
        // Close the app when it loses focus
        disconnectBLE()
        finish()
    }

    override fun onRequestPermissionsResult(
        requestCode: Int,
        permissions: Array<out String>,
        grantResults: IntArray
    ) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults)
        when (requestCode) {
            REQUEST_BLUETOOTH_CONNECT -> {
                if ((grantResults.isNotEmpty() && grantResults[0] == PackageManager.PERMISSION_GRANTED)) {
                    // Permission was granted, proceed with Bluetooth connection
                } else {
                    // Permission denied, handle the case
                }
                return
            }
            // Handle other permissions if there are more
        }
    }

    private fun disconnectBLE() {
        // Check if Bluetooth adapter is available
        if (bluetoothAdapter != null) {
            if (ActivityCompat.checkSelfPermission(
                    this,
                    Manifest.permission.BLUETOOTH_CONNECT
                ) != PackageManager.PERMISSION_GRANTED
            ) {
                // TODO: Consider calling
                //    ActivityCompat#requestPermissions
                // here to request the missing permissions, and then overriding
                //   public void onRequestPermissionsResult(int requestCode, String[] permissions,
                //                                          int[] grantResults)
                // to handle the case where the user grants the permission. See the documentation
                // for ActivityCompat#requestPermissions for more details.
                return
            }
            bluetoothGatt?.disconnect()
        }
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        var scoreTeamA = 0;
        var scoreTeamB = 0;
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        val textbox_sideA: EditText = findViewById(R.id.textbox_sideA)
        val button_AM: ImageButton = findViewById(R.id.button_AM)
        val textbox_sideB: EditText = findViewById(R.id.textbox_sideB)
        val button_BM: ImageButton = findViewById(R.id.button_BM)
        val button_reset: Button = findViewById(R.id.button_reset)
        val display_on_off: Button = findViewById(R.id.display_on_off)
        val change_side: Button = findViewById(R.id.change_side)
        val close_app: Button = findViewById(R.id.close_app)

        if (ActivityCompat.checkSelfPermission(
                this,
                Manifest.permission.BLUETOOTH_CONNECT
            ) != PackageManager.PERMISSION_GRANTED
        ) {
            // Request the permission
            ActivityCompat.requestPermissions(
                this,
                arrayOf(Manifest.permission.BLUETOOTH_CONNECT),
                REQUEST_BLUETOOTH_CONNECT
            )
            return
        }

        bluetoothAdapter = BluetoothAdapter.getDefaultAdapter()
        if (bluetoothAdapter == null) {
            Log.d("BLE debug", "bluetoothAdapter not found")
        } else {
            Log.d("BLE debug", "bluetoothAdapter Found!")
        }

        bluetoothDevice =

            //bluetoothAdapter.getRemoteDevice("C0:89:C0:6B:AD:4B") // Replace with your device's MAC address
            bluetoothAdapter.getRemoteDevice(SCOREBOARD_MAC) // Replace with your device's MAC address
        if (bluetoothDevice == null) {
            Log.d("BLE debug", "bluetoothDevice not found")
        } else {
            Log.d("BLE debug", "bluetoothDevice Found!")
            //Log.d("BLE debug", bluetoothDevice.name)
        }

        bluetoothGatt = bluetoothDevice.connectGatt(this, false, gattCallback)

        if (bluetoothGatt == null) {
            Log.d("BLE debug", "Failed to connect to GATT server.")
        } else {
            Log.d("BLE debug", "Connected to GATT server.")
        }

        button_AM.setOnClickListener {
            val value = textbox_sideA.text.toString()
            val inputNumber: Int = value.toInt()
            writeCharacteristic(inputNumber, CMD_SIDE_A_SET, bluetoothCharacteristicCommand)
        }
        button_BM.setOnClickListener {
            val value = textbox_sideB.text.toString()
            val inputNumber: Int = value.toInt()
            writeCharacteristic(inputNumber, CMD_SIDE_B_SET, bluetoothCharacteristicCommand)
        }
        button_reset.setOnClickListener {
            writeCharacteristic(0, CMD_RESET_COUNTERS, bluetoothCharacteristicCommand)
        }
        display_on_off.setOnClickListener {
            writeCharacteristic(0, CMD_DISPLAY_ON_OFF, bluetoothCharacteristicCommand)
        }
        change_side.setOnClickListener {
            writeCharacteristic(0, CMD_CHANGE_SIDES, bluetoothCharacteristicCommand)
        }
        close_app.setOnClickListener {
            finish()
        }
    }

    fun writeCharacteristic(data: Int, command: Int, bluetoothCharacteristic: BluetoothGattCharacteristic) {
        Log.d("BLE debug", "Write: $data,  $command")


        bluetoothCharacteristic?.value = byteArrayOf(command.toByte(),data.toByte())
        if (ActivityCompat.checkSelfPermission(
                this,
                Manifest.permission.BLUETOOTH_CONNECT
            ) != PackageManager.PERMISSION_GRANTED
        ) {
            // TODO: Consider calling
            //    ActivityCompat#requestPermissions
            // here to request the missing permissions, and then overriding
            //   public void onRequestPermissionsResult(int requestCode, String[] permissions,
            //                                          int[] grantResults)
            // to handle the case where the user grants the permission. See the documentation
            // for ActivityCompat#requestPermissions for more details.
            return
        }
        bluetoothGatt.writeCharacteristic(bluetoothCharacteristic)

    }

    private val gattCallback: BluetoothGattCallback = object : BluetoothGattCallback() {
        override fun onConnectionStateChange(gatt: BluetoothGatt, status: Int, newState: Int) {
            // Handle connection state changes here
            val deviceAddress = gatt.device.address
            if (status == BluetoothGatt.GATT_SUCCESS) {
                if (newState == BluetoothGatt.STATE_CONNECTED) {
                    Log.w("BLE debug BluetoothGattCallback", "Successfully connected to $deviceAddress")
                    if (ActivityCompat.checkSelfPermission(
                            this@MainActivity,
                            Manifest.permission.BLUETOOTH_CONNECT
                        ) != PackageManager.PERMISSION_GRANTED
                    ) {
                        // TODO: Consider calling
                        //    ActivityCompat#requestPermissions
                        // here to request the missing permissions, and then overriding
                        //   public void onRequestPermissionsResult(int requestCode, String[] permissions,
                        //                                          int[] grantResults)
                        // to handle the case where the user grants the permission. See the documentation
                        // for ActivityCompat#requestPermissions for more details.
                        return
                    }
                    gatt.discoverServices();
                    // TODO: Store a reference to BluetoothGatt

                } else if (newState == BluetoothGatt.STATE_DISCONNECTED) {
                    Log.w("BLE debug BluetoothGattCallback", "Successfully disconnected from $deviceAddress")
                    if (ActivityCompat.checkSelfPermission(this@MainActivity,
                            Manifest.permission.BLUETOOTH_CONNECT
                        ) != PackageManager.PERMISSION_GRANTED
                    ) {
                        // TODO: Consider calling
                        //    ActivityCompat#requestPermissions
                        // here to request the missing permissions, and then overriding
                        //   public void onRequestPermissionsResult(int requestCode, String[] permissions,
                        //                                          int[] grantResults)
                        // to handle the case where the user grants the permission. See the documentation
                        // for ActivityCompat#requestPermissions for more details.
                        return
                    }
                    gatt.close()
                }
            } else {
                Log.w("BLE debug BluetoothGattCallback", "Error $status encountered for $deviceAddress! Disconnecting...")
                gatt.close()
            }
        }

        override fun onServicesDiscovered(gatt: BluetoothGatt, status: Int) {
            if (status == BluetoothGatt.GATT_SUCCESS) {
                Log.d("BLE debug", "Services discovered successfully.");
                // Now you can interact with the services
                // Example: gatt.readCharacteristic(characteristic);
                var service = gatt.getService(UUID.fromString(scoreService))
                bluetoothCharacteristicCommand = service?.getCharacteristic(UUID.fromString(commandCharacteristic))!!
            } else {
                Log.d("BLE debug", "Failed to discover services.");
            }
        } // Implement other necessary callback methods here
    }
}

