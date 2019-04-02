// Data server 

import java.net.*;
import java.io.*;

public class DateServer
{
	public static void main(String[] args) {
		try {
			ServerSocket sock = new ServerSocket(3333);

			// now listen for connections
			while (true) {
				Socket client = sock.accept();

				PrintWriter pout = new PrintWriter(client.getOutputStream(), true);

				// write the Date to the socket
				pout.println(new java.util.Date().toString());

				// close the socket and resume
				// listening for connections
				client.close();
			}
		}
		catch (IOException ioe) {
			System.err.println(ioe);
		}
	}
}


// Data client

import java.net.*;
import java.io.*;

public class DateClient
{
	public static void main(String[] args) {
		try {
			// make connection to server socket
			Socket sock = new Socket("127.0.0.1", 3333);

			InputStream in = sock.getInputStream();
			BufferedReader bin = new BufferedReader(new InputStreamReader(in));

			// read the date from the socket
			String line;
			while ((line = bin.readLine()) != null) {
				System.out.println(line);

				// close the socket connection
				sock.close();
			}
		}
		catch (IOException ioe) {
			System.err.println(ioe);
		}
	}
}


	
