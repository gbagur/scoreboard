package com.example.scoreboard2

import android.Manifest
import android.animation.ArgbEvaluator
import android.animation.ValueAnimator
import android.annotation.SuppressLint
import android.bluetooth.BluetoothAdapter
import android.bluetooth.BluetoothDevice
import android.bluetooth.BluetoothGatt
import android.bluetooth.BluetoothGattCallback
import android.bluetooth.BluetoothGattCharacteristic
import android.bluetooth.BluetoothGattDescriptor
import android.bluetooth.BluetoothManager
import android.content.Context
import android.content.Intent
import android.content.pm.PackageManager
import android.content.res.ColorStateList
import android.graphics.Color
import android.os.Bundle
import android.os.Handler
import android.os.Looper
import android.util.Log
import android.view.MotionEvent
import android.view.View
import android.widget.Button
import android.widget.ProgressBar
import android.widget.TextView
import androidx.appcompat.app.AppCompatActivity
import androidx.core.app.ActivityCompat
import java.util.UUID
import android.widget.Toast
import java.util.LinkedList
import java.util.Queue

// List of commands
const val CMD_SIDE_A_INC      = 1
const val CMD_SIDE_A_DEC      = 2
const val CMD_SIDE_B_INC      = 3
const val CMD_SIDE_B_DEC      = 4
const val CMD_SIDE_A_SET      = 5
const val CMD_SIDE_B_SET      = 6
const val CMD_DISPLAY_ON      = 7
const val CMD_DISPLAY_OFF     = 8
const val CMD_RESET_COUNTERS  = 10
const val CMD_DISPLAY_ON_OFF  = 11
const val CMD_CHANGE_SIDES    = 12
const val CMD_SOUND_ON_OFF    = 13
const val CMD_SOUND_ON        = 14
const val CMD_SOUND_OFF       = 15

const val SCOREBOARD_SERVICE_UUID = "45340637-DEA7-48D7-9262-5F392E4317C6"
const val SCOREBOARD_COMMAND_CHARACTERISTIC_UUID = "81044b7b-7fb3-41e1-9127-a8730afd24ff"
const val SCORE_SYNC_CHARACTERISTIC_UUID = "3c870e28-2f8d-4f6c-9742-996914597f8c"

const val BATTERY_SERVICE_UUID = "0000180f-0000-1000-8000-00805f9b34fb"
const val BATTERY_LEVEL_CHARACTERISTIC_UUID = "00002a19-0000-1000-8000-00805f9b34fb"
const val CLIENT_CHARACTERISTIC_CONFIG_UUID = "00002902-0000-1000-8000-00805f9b34fb"

class MainActivity : AppCompatActivity() {
    private val REQUEST_BLUETOOTH_CONNECT = 1
    private lateinit var bluetoothAdapter: BluetoothAdapter
    private var bluetoothDevice: BluetoothDevice? = null
    private var bluetoothGatt: BluetoothGatt? = null
    private var bluetoothCharacteristicCommand: BluetoothGattCharacteristic? = null

    private var scoreA = 0
    private var scoreB = 0

    private lateinit var textViewScoreA: TextView
    private lateinit var textViewScoreB: TextView
    private lateinit var textViewConnectionStatus: TextView
    private lateinit var textViewBattery: TextView
    private lateinit var buttonChangeSides: Button

    // Queue for BLE operations
    private val commandQueue: Queue<ByteArray> = LinkedList()
    private var isWriting = false

    private var blinkAnimator: ValueAnimator? = null

    // Reset button press logic
    private val resetHoldDuration = 1000L
    private val resetTickInterval = 50L
    private var resetPressStartTime = 0L
    private val mainHandler = Handler(Looper.getMainLooper())
    private lateinit var progressBarReset: ProgressBar

    private var targetDeviceAddress: String? = null
    private var isReconnecting = false
    private var isFirstConnection = true
    
    // State machine for setup
    private var setupStep = 0

    private val resetRunnable = object : Runnable {
        override fun run() {
            val elapsedTime = System.currentTimeMillis() - resetPressStartTime
            if (elapsedTime >= resetHoldDuration) {
                performReset()
                stopResetTimer()
            } else {
                progressBarReset.progress = elapsedTime.toInt()
                mainHandler.postDelayed(this, resetTickInterval)
            }
        }
    }

    override fun onPause() {
        super.onPause()
        Log.d("MainActivity", "App is losing focus")
    }

    override fun onDestroy() {
        super.onDestroy()
        disconnectBLE()
    }

    private fun disconnectBLE() {
        if (ActivityCompat.checkSelfPermission(this, Manifest.permission.BLUETOOTH_CONNECT) == PackageManager.PERMISSION_GRANTED) {
            bluetoothGatt?.disconnect()
            bluetoothGatt?.close()
            bluetoothGatt = null
        }
    }

    @SuppressLint("ClickableViewAccessibility")
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        targetDeviceAddress = intent.getStringExtra("DEVICE_ADDRESS")
        if (targetDeviceAddress == null) {
            Toast.makeText(this, "No device address provided", Toast.LENGTH_SHORT).show()
            finish()
            return
        }

        textViewScoreA = findViewById(R.id.textView_scoreA)
        textViewScoreB = findViewById(R.id.textView_scoreB)
        textViewConnectionStatus = findViewById(R.id.textView_connection_status)
        textViewBattery = findViewById(R.id.textView_battery)
        buttonChangeSides = findViewById(R.id.change_side)
        progressBarReset = findViewById(R.id.progressBar_reset)
        progressBarReset.max = resetHoldDuration.toInt()

        findViewById<Button>(R.id.button_A_inc).setOnClickListener {
            stopBlink()
            scoreA++
            updateUI()
            sendBothScores()
        }
        findViewById<Button>(R.id.button_A_dec).setOnClickListener {
            stopBlink()
            if (scoreA > 0) scoreA--
            updateUI()
            sendBothScores()
        }
        findViewById<Button>(R.id.button_B_inc).setOnClickListener {
            stopBlink()
            scoreB++
            updateUI()
            sendBothScores()
        }
        findViewById<Button>(R.id.button_B_dec).setOnClickListener {
            stopBlink()
            if (scoreB > 0) scoreB--
            updateUI()
            sendBothScores()
        }
        
        val buttonReset = findViewById<Button>(R.id.button_reset)
        buttonReset.setOnTouchListener { _, event ->
            when (event.action) {
                MotionEvent.ACTION_DOWN -> {
                    startResetTimer()
                    true
                }
                MotionEvent.ACTION_UP, MotionEvent.ACTION_CANCEL -> {
                    stopResetTimer()
                    true
                }
                else -> false
            }
        }

        findViewById<Button>(R.id.display_on_off).setOnClickListener {
            writeCharacteristic(0, CMD_DISPLAY_ON_OFF)
        }
        findViewById<Button>(R.id.sound_on_off).setOnClickListener {
            writeCharacteristic(0, CMD_SOUND_ON_OFF)
        }
        buttonChangeSides.setOnClickListener {
            stopBlink()
            val temp = scoreA
            scoreA = scoreB
            scoreB = temp
            updateUI()
            writeCharacteristic(0, CMD_CHANGE_SIDES)
        }
        findViewById<Button>(R.id.close_app).setOnClickListener {
            disconnectBLE()
            val intent = Intent(this, ConnectActivity::class.java)
            startActivity(intent)
            finish()
        }

        val bluetoothManager = getSystemService(Context.BLUETOOTH_SERVICE) as BluetoothManager
        bluetoothAdapter = bluetoothManager.adapter

        if (ActivityCompat.checkSelfPermission(this, Manifest.permission.BLUETOOTH_CONNECT) != PackageManager.PERMISSION_GRANTED) {
            ActivityCompat.requestPermissions(this, arrayOf(Manifest.permission.BLUETOOTH_CONNECT), REQUEST_BLUETOOTH_CONNECT)
        } else {
            connectToDevice(targetDeviceAddress!!)
        }
    }

    private fun startResetTimer() {
        resetPressStartTime = System.currentTimeMillis()
        progressBarReset.progress = 0
        progressBarReset.visibility = View.VISIBLE
        mainHandler.post(resetRunnable)
    }

    private fun stopResetTimer() {
        mainHandler.removeCallbacks(resetRunnable)
        progressBarReset.visibility = View.INVISIBLE
        progressBarReset.progress = 0
    }

    private fun performReset() {
        stopBlink()
        scoreA = 0
        scoreB = 0
        updateUI()
        writeCharacteristic(0, CMD_RESET_COUNTERS)
        writeCharacteristic(0, CMD_DISPLAY_ON)
        Toast.makeText(this, "Scores Reset", Toast.LENGTH_SHORT).show()
    }

    private fun updateUI() {
        textViewScoreA.text = scoreA.toString()
        textViewScoreB.text = scoreB.toString()
        checkScoreForBlink()
    }

    private fun checkScoreForBlink() {
        val totalScore = scoreA + scoreB
        if (totalScore > 0 && totalScore % 7 == 0) {
            startBlink()
        } else {
            stopBlink()
        }
    }

    private fun startBlink() {
        if (blinkAnimator != null) return // Already blinking

        val originalColor = getColor(R.color.buttons)
        val highlightColor = Color.YELLOW 

        blinkAnimator = ValueAnimator.ofObject(ArgbEvaluator(), originalColor, highlightColor).apply {
            duration = 600
            repeatCount = ValueAnimator.INFINITE
            repeatMode = ValueAnimator.REVERSE
            addUpdateListener { animator ->
                buttonChangeSides.backgroundTintList = ColorStateList.valueOf(animator.animatedValue as Int)
            }
            start()
        }
    }

    private fun stopBlink() {
        blinkAnimator?.cancel()
        blinkAnimator = null
        buttonChangeSides.backgroundTintList = ColorStateList.valueOf(getColor(R.color.buttons))
    }

    private fun sendBothScores() {
        writeCharacteristic(scoreA, CMD_SIDE_A_SET)
        writeCharacteristic(scoreB, CMD_SIDE_B_SET)
    }

    private fun connectToDevice(address: String) {
        runOnUiThread {
            textViewConnectionStatus.text = "Status: Connecting..."
            textViewConnectionStatus.setTextColor(Color.YELLOW)
        }
        bluetoothDevice = bluetoothAdapter.getRemoteDevice(address)
        if (ActivityCompat.checkSelfPermission(this, Manifest.permission.BLUETOOTH_CONNECT) == PackageManager.PERMISSION_GRANTED) {
            bluetoothGatt = bluetoothDevice?.connectGatt(this, false, gattCallback)
        }
    }

    private fun retryConnection() {
        if (isReconnecting) return
        isReconnecting = true
        mainHandler.postDelayed({
            isReconnecting = false
            targetDeviceAddress?.let { connectToDevice(it) }
        }, 3000)
    }

    @Synchronized
    fun writeCharacteristic(data: Int, command: Int) {
        val payload = byteArrayOf(command.toByte(), data.toByte())
        commandQueue.add(payload)
        processQueue()
    }

    @Synchronized
    private fun processQueue() {
        if (isWriting || commandQueue.isEmpty()) return
        
        val char = bluetoothCharacteristicCommand ?: return
        val gatt = bluetoothGatt ?: return
        
        val payload = commandQueue.peek() ?: return
        isWriting = true
        
        char.value = payload
        
        if (ActivityCompat.checkSelfPermission(this, Manifest.permission.BLUETOOTH_CONNECT) == PackageManager.PERMISSION_GRANTED) {
            val success = gatt.writeCharacteristic(char)
            if (success) {
                commandQueue.poll() 
            } else {
                Log.e("BLE", "Failed to initiate write for command ${payload[0]}, will retry...")
                isWriting = false
                mainHandler.postDelayed({ processQueue() }, 100)
            }
        } else {
            isWriting = false
        }
    }

    private val gattCallback: BluetoothGattCallback = object : BluetoothGattCallback() {
        override fun onConnectionStateChange(gatt: BluetoothGatt, status: Int, newState: Int) {
            if (status == BluetoothGatt.GATT_SUCCESS) {
                if (newState == BluetoothGatt.STATE_CONNECTED) {
                    runOnUiThread {
                        textViewConnectionStatus.text = "Status: Connected"
                        textViewConnectionStatus.setTextColor(Color.GREEN)
                    }
                    if (ActivityCompat.checkSelfPermission(this@MainActivity, Manifest.permission.BLUETOOTH_CONNECT) == PackageManager.PERMISSION_GRANTED) {
                        gatt.discoverServices()
                    }
                } else if (newState == BluetoothGatt.STATE_DISCONNECTED) {
                    handleDisconnect(gatt)
                }
            } else {
                Log.e("BLE", "GATT error status: $status")
                handleDisconnect(gatt)
            }
        }

        private fun handleDisconnect(gatt: BluetoothGatt) {
            runOnUiThread {
                textViewConnectionStatus.text = "Status: Disconnected"
                textViewConnectionStatus.setTextColor(Color.RED)
                textViewBattery.text = "Battery: --%"
            }
            bluetoothCharacteristicCommand = null
            if (ActivityCompat.checkSelfPermission(this@MainActivity, Manifest.permission.BLUETOOTH_CONNECT) == PackageManager.PERMISSION_GRANTED) {
                gatt.close()
            }
            bluetoothGatt = null
            setupStep = 0
            retryConnection()
        }

        override fun onServicesDiscovered(gatt: BluetoothGatt, status: Int) {
            if (status == BluetoothGatt.GATT_SUCCESS) {
                Log.d("BLE", "Services discovered")
                val service = gatt.getService(UUID.fromString(SCOREBOARD_SERVICE_UUID))
                bluetoothCharacteristicCommand = service?.getCharacteristic(UUID.fromString(SCOREBOARD_COMMAND_CHARACTERISTIC_UUID))
                
                setupStep = 1
                nextSetupStep(gatt)
            }
        }

        private fun nextSetupStep(gatt: BluetoothGatt) {
            if (ActivityCompat.checkSelfPermission(this@MainActivity, Manifest.permission.BLUETOOTH_CONNECT) != PackageManager.PERMISSION_GRANTED) return

            when (setupStep) {
                1 -> {
                    // Enable Battery notifications
                    val batteryService = gatt.getService(UUID.fromString(BATTERY_SERVICE_UUID))
                    val batteryLevelChar = batteryService?.getCharacteristic(UUID.fromString(BATTERY_LEVEL_CHARACTERISTIC_UUID))
                    if (batteryLevelChar != null) {
                        gatt.setCharacteristicNotification(batteryLevelChar, true)
                        val descriptor = batteryLevelChar.getDescriptor(UUID.fromString(CLIENT_CHARACTERISTIC_CONFIG_UUID))
                        if (descriptor != null) {
                            descriptor.value = BluetoothGattDescriptor.ENABLE_NOTIFICATION_VALUE
                            isWriting = true
                            gatt.writeDescriptor(descriptor)
                        } else {
                            setupStep = 2
                            nextSetupStep(gatt)
                        }
                    } else {
                        setupStep = 2
                        nextSetupStep(gatt)
                    }
                }
                2 -> {
                    // Enable Score Sync notifications
                    val service = gatt.getService(UUID.fromString(SCOREBOARD_SERVICE_UUID))
                    val scoreSyncChar = service?.getCharacteristic(UUID.fromString(SCORE_SYNC_CHARACTERISTIC_UUID))
                    if (scoreSyncChar != null) {
                        gatt.setCharacteristicNotification(scoreSyncChar, true)
                        val descriptor = scoreSyncChar.getDescriptor(UUID.fromString(CLIENT_CHARACTERISTIC_CONFIG_UUID))
                        if (descriptor != null) {
                            descriptor.value = BluetoothGattDescriptor.ENABLE_NOTIFICATION_VALUE
                            isWriting = true
                            gatt.writeDescriptor(descriptor)
                        } else {
                            setupStep = 3
                            nextSetupStep(gatt)
                        }
                    } else {
                        setupStep = 3
                        nextSetupStep(gatt)
                    }
                }
                3 -> {
                    // Initial Score Read
                    val service = gatt.getService(UUID.fromString(SCOREBOARD_SERVICE_UUID))
                    val scoreSyncChar = service?.getCharacteristic(UUID.fromString(SCORE_SYNC_CHARACTERISTIC_UUID))
                    if (scoreSyncChar != null && isFirstConnection) {
                        isWriting = true
                        gatt.readCharacteristic(scoreSyncChar)
                    } else {
                        setupStep = 4
                        nextSetupStep(gatt)
                    }
                }
                4 -> {
                    // Start processing Command Queue
                    runOnUiThread {
                        if (isFirstConnection) {
                            // If we didn't manage to read score, we'd reset here, but we'll trust the read for now.
                            // However, we still want to ensure display is ON.
                            writeCharacteristic(0, CMD_DISPLAY_ON)
                            isFirstConnection = false
                        } else {
                            sendBothScores()
                        }
                        isWriting = false
                        processQueue()
                    }
                }
            }
        }

        override fun onDescriptorWrite(gatt: BluetoothGatt, descriptor: BluetoothGattDescriptor, status: Int) {
            Log.d("BLE", "Descriptor write status: $status for ${descriptor.uuid}")
            isWriting = false
            setupStep++
            nextSetupStep(gatt)
        }

        override fun onCharacteristicRead(gatt: BluetoothGatt, characteristic: BluetoothGattCharacteristic, status: Int) {
            isWriting = false
            if (status == BluetoothGatt.GATT_SUCCESS) {
                when (characteristic.uuid) {
                    UUID.fromString(BATTERY_LEVEL_CHARACTERISTIC_UUID) -> {
                        val batteryLevel = characteristic.getIntValue(BluetoothGattCharacteristic.FORMAT_UINT8, 0)
                        Log.d("BLE", "Battery Level Read: $batteryLevel")
                        runOnUiThread {
                            textViewBattery.text = "Battery: $batteryLevel%"
                        }
                    }
                    UUID.fromString(SCORE_SYNC_CHARACTERISTIC_UUID) -> {
                        val combinedScore = characteristic.getIntValue(BluetoothGattCharacteristic.FORMAT_UINT16, 0)
                        scoreA = combinedScore and 0xFF
                        scoreB = (combinedScore shr 8) and 0xFF
                        Log.d("BLE", "Score Sync Read: $scoreA - $scoreB")
                        runOnUiThread {
                            updateUI()
                        }
                    }
                }
            }
            if (setupStep == 3) {
                setupStep++
                nextSetupStep(gatt)
            }
        }

        override fun onCharacteristicChanged(gatt: BluetoothGatt, characteristic: BluetoothGattCharacteristic) {
            when (characteristic.uuid) {
                UUID.fromString(BATTERY_LEVEL_CHARACTERISTIC_UUID) -> {
                    val batteryLevel = characteristic.getIntValue(BluetoothGattCharacteristic.FORMAT_UINT8, 0)
                    Log.d("BLE", "Battery Level Changed: $batteryLevel")
                    runOnUiThread {
                        textViewBattery.text = "Battery: $batteryLevel%"
                    }
                }
                UUID.fromString(SCORE_SYNC_CHARACTERISTIC_UUID) -> {
                    val combinedScore = characteristic.getIntValue(BluetoothGattCharacteristic.FORMAT_UINT16, 0)
                    scoreA = combinedScore and 0xFF
                    scoreB = (combinedScore shr 8) and 0xFF
                    Log.d("BLE", "Score Sync Changed: $scoreA - $scoreB")
                    runOnUiThread {
                        updateUI()
                    }
                }
            }
        }

        override fun onCharacteristicWrite(gatt: BluetoothGatt, characteristic: BluetoothGattCharacteristic, status: Int) {
            Log.d("BLE", "Write completed with status: $status")
            isWriting = false
            runOnUiThread { processQueue() }
        }
    }
}
