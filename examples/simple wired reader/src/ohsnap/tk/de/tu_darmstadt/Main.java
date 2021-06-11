package ohsnap.tk.de.tu_darmstadt;

import java.io.BufferedReader;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStreamReader;

import com.fazecast.jSerialComm.SerialPort;

public class Main {

	private static final int CALIB_SECS = 10;
	private static final byte PASSES = 10;
	private static final String ANSI_RESET = "\u001B[0m";
	public static final String ANSI_GREEN = "\u001B[32m";
	public static final String ANSI_YELLOW = "\u001B[43m";
	
	public static void main(String[] args) {
		//clear screen
		System.out.print("\033[H\033[2J");  
	    System.out.flush(); 
		
		System.out.println(ANSI_GREEN + "Welcome to the Oh Snap user study interface test!" + ANSI_RESET);
		System.out.println(ANSI_GREEN + "---------------------------------------------------" + ANSI_RESET);
		System.out.println(ANSI_YELLOW + "DO NOT DISCONNECT USB DURING THIS TEST" + ANSI_RESET);
		// setup comPort
		System.out.print("Setting up UART interface ...");
		SerialPort comPorts[] = SerialPort.getCommPorts();
		if(comPorts.length == 0) {
			System.out.print("No device connected to UART. Exiting now.");
			return;
		}
		// assuming the board is the only connected device
		SerialPort comPort = comPorts[0]; 
		comPort.setBaudRate(115200);
		System.out.println(" UART connected (" + comPort.getPortDescription() + " on "+ comPort.getSystemPortName() +")");

			
		System.out.println("Please type in your name:");
		BufferedReader reader = new BufferedReader(new InputStreamReader(System.in));
		String name = "";
		try {
			name = reader.readLine();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			System.out.println("invalid input!");
			e.printStackTrace();
			return;
		}
		System.out.println("Hello " + name);
		
		
		String fileName = name + "_idle.txt";
		FileWriter fileWriter = createFile(fileName); 
		if(fileWriter==null) {
			System.out.println("File " + fileName + "could not be created. Exiting now.");
			return;
		}
		monitor(name, -1, comPort, fileWriter);
		System.out.println("Completed capturing idle values!");
		try {
			fileWriter.close();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
		fileName = name + ".txt";
		fileWriter = createFile(fileName); 
		if(fileWriter==null) {
			System.out.println("File " + fileName + "could not be created. Exiting now.");
			return;
		}

		for(byte pass = 0; pass < PASSES; pass++) {
			monitor(name, pass, comPort, fileWriter);
			System.out.println("Pass " + (pass + 1) + "/" + PASSES + " completed!");
		}
		try {
			fileWriter.close();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		System.out.println("\r\nFile successfully written.");
		comPort.closePort();
		System.out.println("Test finished.");

	}
		
	static FileWriter createFile(String fileName) {
		
		FileWriter fileWriter = null; 
		try {
			fileWriter = new FileWriter(fileName);
			fileWriter.write("participant,pass,t0,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11\r\n");
		} catch (IOException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		}
		return fileWriter;
	}

	
	static void monitor(String userName, int pass, SerialPort comPort, FileWriter fileWriter) {
		if(pass == -1) {
			System.out.println("Please disconnect the board from the 3D printed interface. Afterwards press enter.");	
		}else {
			System.out.println("Now (re)connect the board to the 3D printed interface. Afterwards press enter.");
		}
		try {
			System.in.read();
		} catch (IOException e2) {
			// TODO Auto-generated catch block
			e2.printStackTrace();
		}
				
		System.out.println("Waiting for sensors to settle down");
		for(int sec = CALIB_SECS; sec>0; sec--) {
			try {
				Thread.sleep(1000);
				System.out.print(sec + "...");
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}		
		}
		System.out.println();
		try {
			System.out.print("Opening Port ... ");
			comPort.openPort();
			System.out.println("OK! Reading from " + comPort.getPortDescription());
			int lines = 0;
			while (lines < 100) {
				while (comPort.bytesAvailable() == 0) {
					Thread.sleep(20);
				}
				if (comPort.bytesAvailable() != -1) {
					if(pass==-1) {
						fileWriter.write(userName+ ",idle,");
					}else {
						fileWriter.write(userName+ "," + pass + ",");
					}
					byte[] readBuffer = new byte[comPort.bytesAvailable()];
					comPort.readBytes(readBuffer, readBuffer.length);
					String text = new String(readBuffer, "UTF-8");
					System.out.print(text);
					fileWriter.write(text);
					lines++;
				}
			}
		} catch (Exception e) {
			e.printStackTrace();
		}
		comPort.closePort();
	}

}
