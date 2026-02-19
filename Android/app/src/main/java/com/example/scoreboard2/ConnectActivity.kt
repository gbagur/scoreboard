package com.example.scoreboard2

import android.Manifest
import android.bluetooth.BluetoothAdapter
import android.bluetooth.BluetoothDevice
import android.bluetooth.BluetoothManager
import android.bluetooth.le.ScanCallback
import android.bluetooth.le.ScanResult
import android.bluetooth.le.ScanSettings
import android.content.Context
import android.content.Intent
import android.content.pm.PackageManager
import android.os.Build
import android.os.Bundle
import android.os.Handler
import android.os.Looper
import android.util.Log
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.ArrayAdapter
import android.widget.Button
import android.widget.ListView
import android.widget.TextView
import android.widget.Toast
import androidx.appcompat.app.AppCompatActivity
import androidx.core.app.ActivityCompat

class ConnectActivity : AppCompatActivity() {

    private lateinit var bluetoothAdapter: BluetoothAdapter
    private val devices = mutableListOf<BluetoothDevice>()
    private lateinit var adapter: DeviceAdapter
    private val handler = Handler(Looper.getMainLooper())
    private var scanning = false

    private val scanCallback = object : ScanCallback() {
        override fun onScanResult(callbackType: Int, result: ScanResult) {
            val device = result.device
            
            // Get name from device object or scan record
            val deviceName = if (ActivityCompat.checkSelfPermission(this@ConnectActivity, Manifest.permission.BLUETOOTH_CONNECT) == PackageManager.PERMISSION_GRANTED || Build.VERSION.SDK_INT < Build.VERSION_CODES.S) {
                device.name ?: result.scanRecord?.deviceName
            } else {
                result.scanRecord?.deviceName
            }

            Log.d("ScanResult", "Found: ${deviceName ?: "Unknown"} [${device.address}]")

            // Filter for "ScoreBoard" as requested
            if (deviceName != null && deviceName.contains("ScoreBoard", ignoreCase = true)) {
                runOnUiThread {
                    if (!devices.any { it.address == device.address }) {
                        devices.add(device)
                        adapter.notifyDataSetChanged()
                    }
                }
            }
        }

        override fun onScanFailed(errorCode: Int) {
            Log.e("ScanResult", "onScanFailed: $errorCode")
        }
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_connect)

        val bluetoothManager = getSystemService(Context.BLUETOOTH_SERVICE) as BluetoothManager
        bluetoothAdapter = bluetoothManager.adapter

        val listView = findViewById<ListView>(R.id.deviceListView)
        adapter = DeviceAdapter(this, devices)
        listView.adapter = adapter

        listView.setOnItemClickListener { _, _, position, _ ->
            val device = devices[position]
            stopScan()
            val intent = Intent(this, MainActivity::class.java).apply {
                putExtra("DEVICE_ADDRESS", device.address)
            }
            startActivity(intent)
            finish()
        }

        findViewById<Button>(R.id.scanButton).setOnClickListener {
            checkPermissions()
        }

        checkPermissions()
    }

    private fun checkPermissions() {
        val permissions = mutableListOf<String>()
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
            permissions.add(Manifest.permission.BLUETOOTH_SCAN)
            permissions.add(Manifest.permission.BLUETOOTH_CONNECT)
            permissions.add(Manifest.permission.ACCESS_FINE_LOCATION)
        } else {
            permissions.add(Manifest.permission.ACCESS_FINE_LOCATION)
            permissions.add(Manifest.permission.ACCESS_COARSE_LOCATION)
        }

        val missingPermissions = permissions.filter {
            ActivityCompat.checkSelfPermission(this, it) != PackageManager.PERMISSION_GRANTED
        }

        if (missingPermissions.isNotEmpty()) {
            ActivityCompat.requestPermissions(this, missingPermissions.toTypedArray(), 101)
        } else {
            startScan()
        }
    }

    override fun onRequestPermissionsResult(requestCode: Int, permissions: Array<out String>, grantResults: IntArray) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults)
        if (requestCode == 101 && grantResults.all { it == PackageManager.PERMISSION_GRANTED }) {
            startScan()
        }
    }

    private fun startScan() {
        if (scanning) return
        
        val scanner = bluetoothAdapter.bluetoothLeScanner
        if (scanner == null) return

        devices.clear()
        adapter.notifyDataSetChanged()
        
        scanning = true
        findViewById<View>(R.id.progressBar).visibility = View.VISIBLE
        
        val settings = ScanSettings.Builder()
            .setScanMode(ScanSettings.SCAN_MODE_LOW_LATENCY)
            .build()
            
        scanner.startScan(null, settings, scanCallback)

        handler.postDelayed({
            stopScan()
        }, 10000) 
    }

    private fun stopScan() {
        if (!scanning) return
        scanning = false
        findViewById<View>(R.id.progressBar).visibility = View.GONE
        
        bluetoothAdapter.bluetoothLeScanner?.stopScan(scanCallback)
    }

    private class DeviceAdapter(context: Context, private val devices: List<BluetoothDevice>) :
        ArrayAdapter<BluetoothDevice>(context, android.R.layout.simple_list_item_2, devices) {

        override fun getView(position: Int, convertView: View?, parent: ViewGroup): View {
            val view = convertView ?: LayoutInflater.from(context).inflate(android.R.layout.simple_list_item_2, parent, false)
            val device = devices[position]
            
            val text1 = view.findViewById<TextView>(android.R.id.text1)
            val text2 = view.findViewById<TextView>(android.R.id.text2)

            if (ActivityCompat.checkSelfPermission(context, Manifest.permission.BLUETOOTH_CONNECT) == PackageManager.PERMISSION_GRANTED || Build.VERSION.SDK_INT < Build.VERSION_CODES.S) {
                text1.text = device.name ?: "ScoreBoard" // Fallback since we filtered for it
            } else {
                text1.text = "Permission required"
            }
            text2.text = device.address

            text1.setTextColor(context.getColor(R.color.white))
            text2.setTextColor(context.getColor(android.R.color.darker_gray))

            return view
        }
    }
}
