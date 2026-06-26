import java.awt.*;
import java.awt.event.ActionEvent;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import javax.swing.*;

public class DataAcquisitionGUI extends JFrame {
    private final JButton startButton;
    private final JButton stopButton;
    private final JButton restartButton;
    private final JTextArea logArea;
    private Process acquisitionProcess;
    private Process plottingProcess;
    private boolean isRunning = false;

    public DataAcquisitionGUI() {
        setTitle("FPGA Data Acquisition System");
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        setSize(800, 600);
        setLayout(new BorderLayout());

        // Create buttons panel
        JPanel buttonPanel = new JPanel();
        startButton = new JButton("Start Acquisition");
        stopButton = new JButton("Stop Acquisition");
        restartButton = new JButton("Restart Acquisition");

        buttonPanel.add(startButton);
        buttonPanel.add(stopButton);
        buttonPanel.add(restartButton);

        // Create log area
        logArea = new JTextArea();
        logArea.setEditable(false);
        JScrollPane scrollPane = new JScrollPane(logArea);

        // Add components to frame
        add(buttonPanel, BorderLayout.NORTH);
        add(scrollPane, BorderLayout.CENTER);

        // Set button actions
        startButton.addActionListener((ActionEvent e) -> {
            startAcquisition();
        });

        stopButton.addActionListener((ActionEvent e) -> {
            stopAcquisition();
        });

        restartButton.addActionListener((ActionEvent e) -> {
            restartAcquisition();
        });

        // Disable stop button initially
        stopButton.setEnabled(false);
    }

    private void startAcquisition() {
        if (!isRunning) {
            logMessage("Starting data acquisition...");
            try {
                // Start the Python data acquisition script
                acquisitionProcess = new ProcessBuilder("python", "auto_Rx_file.py").start();
                
                // Start a thread to read the output
                new Thread(() -> {
                    try (BufferedReader reader = new BufferedReader(
                            new InputStreamReader(acquisitionProcess.getInputStream()))) {
                        String line;
                        while ((line = reader.readLine()) != null) {
                            logMessage(line);
                        }
                    } catch (IOException e) {
                        logMessage("Error reading process output: " + e.getMessage());
                    }
                }).start();
                
                isRunning = true;
                startButton.setEnabled(false);
                stopButton.setEnabled(true);
                restartButton.setEnabled(false);
                logMessage("Data acquisition started successfully.");
            } catch (IOException e) {
                logMessage("Failed to start acquisition: " + e.getMessage());
            }
        } else {
            logMessage("Acquisition is already running.");
        }
    }

    private void stopAcquisition() {
        if (isRunning && acquisitionProcess != null) {
            logMessage("Stopping data acquisition...");
            acquisitionProcess.destroy();
            isRunning = false;
            startButton.setEnabled(true);
            stopButton.setEnabled(false);
            restartButton.setEnabled(true);
            logMessage("Data acquisition stopped.");
        }
    }

    private void restartAcquisition() {
        logMessage("Restarting data acquisition...");
        stopAcquisition();
        
        // Add any cleanup of data files if needed here
        
        startAcquisition();
    }

    private void logMessage(String message) {
        SwingUtilities.invokeLater(() -> {
            logArea.append(message + "\n");
            logArea.setCaretPosition(logArea.getDocument().getLength());
        });
    }



    private void plotData() {
        logMessage("Plotting data...");
        try {
            // Compile the C++ code if not already compiled
            Process compileProcess = new ProcessBuilder("g++", "data_frame_reader.C", "-o", "data_reader", 
                    "-lstdc++", "-lCore", "-lImt", "-lRIO", "-lNet", "-lHist", "-lGraf", "-lGpad", "-lTree", "-lMathCore")
                    .start();
            
            compileProcess.waitFor();
            
            // Run the compiled C++ program
            plottingProcess = new ProcessBuilder("./data_reader").start();
            
            // Start a thread to read the output
            new Thread(() -> {
                try (BufferedReader reader = new BufferedReader(
                        new InputStreamReader(plottingProcess.getInputStream()))) {
                    String line;
                    while ((line = reader.readLine()) != null) {
                        logMessage(line);
                    }
                } catch (IOException e) {
                    logMessage("Error reading plot output: " + e.getMessage());
                }
            }).start();
            
            logMessage("Plotting started successfully.");
        } catch (IOException | InterruptedException e) {
            logMessage("Failed to start plotting: " + e.getMessage());
        }
    }

    public static void main(String[] args) {
        SwingUtilities.invokeLater(() -> {
            DataAcquisitionGUI gui = new DataAcquisitionGUI();
            gui.setVisible(true);
        });
    }

    public JButton getStartButton() {
        return startButton;
    }
}