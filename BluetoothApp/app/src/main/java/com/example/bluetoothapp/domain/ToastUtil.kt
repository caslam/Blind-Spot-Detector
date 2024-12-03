package com.example.bluetoothapp.domain

import android.annotation.SuppressLint
import android.content.Context
import android.widget.Toast

class ToastUtil(private val context: Context) {

    private var toast: Toast? = null
    private var isToastVisible = false

    @SuppressLint("NewApi")
    fun showToast(message: String) {
        // If a Toast is already visible, cancel it first
        toast?.cancel()

        // Create a new Toast
        toast = Toast.makeText(context, message, Toast.LENGTH_SHORT)
        toast?.show()

        // Set the flag to indicate a Toast is being displayed
        isToastVisible = true

        // Reset the flag when the Toast is done displaying
        // This will be done in a Runnable after the Toast duration has passed
        toast?.addCallback(object : Toast.Callback() {
            override fun onToastShown() {
                // Toast has been shown, set the flag to true
                isToastVisible = true
            }

            override fun onToastHidden() {
                // Toast has finished, set the flag to false
                isToastVisible = false
            }
        })
    }

    // This function checks if the Toast is currently being displayed
    fun isToastShowing(): Boolean {
        return isToastVisible
    }
}