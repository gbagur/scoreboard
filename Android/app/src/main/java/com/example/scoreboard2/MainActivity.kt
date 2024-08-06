package com.example.scoreboard2

import android.Manifest
import android.bluetooth.BluetoothAdapter
import android.bluetooth.BluetoothDevice
import android.bluetooth.BluetoothGatt
import android.bluetooth.BluetoothGattCallback
import android.bluetooth.BluetoothGattCharacteristic
import android.bluetooth.BluetoothGattService
import android.content.pm.PackageManager
import android.os.Build
import android.os.Bundle
import android.util.Log
import android.widget.Button
import android.widget.ImageButton
import androidx.appcompat.app.AppCompatActivity
import androidx.core.app.ActivityCompat
import androidx.core.content.ContextCompat
import java.util.UUID



class MainActivity : AppCompatActivity() {
    private val REQUEST_BLUETOOTH_CONNECT = 1
    private lateinit var bluetoothAdapter: BluetoothAdapter
    private lateinit var bluetoothDevice: BluetoothDevice
    private lateinit var bluetoothGatt: BluetoothGatt
    lateinit var bluetoothService: BluetoothGattService
    lateinit var bluetoothCharacteristicTeamA: BluetoothGattCharacteristic
    lateinit var bluetoothCharacteristicTeamB: BluetoothGattCharacteristic



    private val scoreService = "45340637-DEA7-48D7-9262-5F392E4317C6"
    private val teamAScoreCharacteristic = "81044b7b-7fb3-41e1-9127-a8730afd24ff"
    private val teamBScoreCharacteristic = "6f61f273-a5c5-4c6a-8744-8ef84a1484c4"

    //private val BLUETOOTH_PERMISSION_CODE = 123

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

        //val connectButton: Button = findViewById(R.id.connectButton)
        val button_AP: ImageButton = findViewById(R.id.button_AP)
        val button_AM: ImageButton = findViewById(R.id.button_AM)
        val button_BP: ImageButton = findViewById(R.id.button_BP)
        val button_BM: ImageButton = findViewById(R.id.button_BM)
        val button_reset: Button = findViewById(R.id.button_reset)
        val display_on_off: Button = findViewById(R.id.display_on_off)
        val change_side: Button = findViewById(R.id.change_side)
        val close_app: Button = findViewById(R.id.close_app)


        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            // Check if Bluetooth permissions are granted
            if (ContextCompat.checkSelfPermission(
                    this,
                    Manifest.permission.BLUETOOTH
                ) != PackageManager.PERMISSION_GRANTED
            ) {
                // Request Bluetooth permissions


            }
        }

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
            Log.d("Gabi", "bluetoothAdapter not found")
        } else {
            Log.d("Gabi", "bluetoothAdapter Found!")
        }

        bluetoothDevice =

            //bluetoothAdapter.getRemoteDevice("C0:89:C0:6B:AD:4B") // Replace with your device's MAC address
            bluetoothAdapter.getRemoteDevice("C0:C3:45:37:24:51") // Replace with your device's MAC address
        if (bluetoothDevice == null) {
            Log.d("Gabi", "bluetoothDevice not found")
        } else {
            Log.d("Gabi", "bluetoothDevice Found!")
            //Log.d("Gabi", bluetoothDevice.name)
        }

        bluetoothGatt = bluetoothDevice.connectGatt(this, false, gattCallback)

        if (bluetoothGatt == null) {
            Log.d("Gabi", "Failed to connect to GATT server.")
        } else {
            Log.d("Gabi", "Connected to GATT server.")
        }


        button_AP.setOnClickListener {
            writeCharacteristic(1, 1, bluetoothCharacteristicTeamA)
        }
        button_AM.setOnClickListener {
            writeCharacteristic(1, 2, bluetoothCharacteristicTeamA)
        }
        button_BP.setOnClickListener {
            writeCharacteristic(0, 3, bluetoothCharacteristicTeamA)
        }
        button_BM.setOnClickListener {
            writeCharacteristic(0, 4, bluetoothCharacteristicTeamA)
        }
        button_reset.setOnClickListener {
            writeCharacteristic(0, 10, bluetoothCharacteristicTeamA)
        }
        display_on_off.setOnClickListener {
            writeCharacteristic(0, 11, bluetoothCharacteristicTeamA)
        }
        change_side.setOnClickListener {
            writeCharacteristic(0, 12, bluetoothCharacteristicTeamA)
        }
        close_app.setOnClickListener {
            finish()
        }

    }


    fun writeCharacteristic(team: Int, value: Int, bluetoothCharacteristic: BluetoothGattCharacteristic) {
        Log.d("Gabi", "Write char to $value")

        bluetoothCharacteristic?.value = byteArrayOf(value.toByte())
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
                    Log.w("Gabi BluetoothGattCallback", "Successfully connected to $deviceAddress")
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
                    Log.w("Gabi BluetoothGattCallback", "Successfully disconnected from $deviceAddress")
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
                Log.w("Gabi BluetoothGattCallback", "Error $status encountered for $deviceAddress! Disconnecting...")
                gatt.close()
            }
        }

        override fun onServicesDiscovered(gatt: BluetoothGatt, status: Int) {
            if (status == BluetoothGatt.GATT_SUCCESS) {
                Log.d("Gabi", "Services discovered successfully.");
                // Now you can interact with the services
                // Example: gatt.readCharacteristic(characteristic);
                var service = gatt.getService(UUID.fromString(scoreService))
                bluetoothCharacteristicTeamA = service?.getCharacteristic(UUID.fromString(teamAScoreCharacteristic))!!
                //bluetoothCharacteristicTeamB = service?.getCharacteristic(UUID.fromString(teamBScoreCharacteristic))!!
                //writeCharacteristic(1, 0, bluetoothCharacteristicTeamA)
                //writeCharacteristic(1, 0, bluetoothCharacteristicTeamB)
            } else {
                Log.d("Gabi", "Failed to discover services.");
            }
        } // Implement other necessary callback methods here
    }
}

