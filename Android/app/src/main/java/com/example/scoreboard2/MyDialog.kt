    package com.example.scoreboard2

    import android.content.Context
    import androidx.appcompat.app.AlertDialog
    import java.util.concurrent.Semaphore
    import kotlinx.coroutines.Dispatchers
    import kotlinx.coroutines.GlobalScope
    import kotlinx.coroutines.launch

    object MyDialog {
        private val semaphore = Semaphore(0)

        fun showMessage(
            context: Context,
            title: String,
            message: String,
            callback: () -> Unit // Callback function that takes no parameters and returns Unit
        ) {
            GlobalScope.launch(Dispatchers.Main) {
                showDialog(context, title, message, callback)
            }
        }

        private suspend fun showDialog(
            context: Context,
            title: String,
            message: String,
            callback: () -> Unit
        ) {
            val builder = AlertDialog.Builder(context)
            builder.setTitle(title)
                .setMessage(message)
                .setPositiveButton("OK") { dialog, _ ->
                    dialog.dismiss()
                    callback() // Invoke the callback function when "OK" is clicked
                    semaphore.release()
                }
                .setNegativeButton("Cancel") { dialog, _ ->
                    dialog.dismiss()
                    callback() // Invoke the callback function when "Cancel" is clicked
                    semaphore.release()
                }
                .setOnCancelListener {
                    // If the dialog is canceled (e.g., back button pressed), invoke the callback
                    callback()
                    semaphore.release()
                }
                .show()
        }
    }