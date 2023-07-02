import java.awt.*;
import java.io.*;
import java.net.Socket;
import java.net.SocketTimeoutException;
import java.util.ArrayList;
import java.util.List;
import java.util.Scanner;

import static java.lang.Integer.parseInt;

public class Client {
    private static Socket socket;

    //Downloading file
    public static void receive(String fileName, ObjectInputStream is){
        try {
            String path="Downloads/";
            File dir=new File(path);

            if(!dir.exists()){
                dir.mkdir();
            }

            long fileSize=(long) is.readObject();
            System.out.println("Downloading "+fileName+" of size "+fileSize);
            File file=new File("Downloads/"+fileName);
            FileOutputStream fos=new FileOutputStream(file);
            BufferedOutputStream bos=new BufferedOutputStream(fos);

            String ack="";
            int bytesRead;

            while(true){
                Object o=is.readObject();
                if(o.getClass().equals(ack.getClass())){
                    ack=(String)o;
                    break;
                }
                byte[] con=(byte[]) o;
                bytesRead=con.length;
                bos.write(con,0,bytesRead);
            }
            bos.flush();
            bos.close();
            fos.close();

            System.out.println(ack);
            if(ack.equalsIgnoreCase("COMPLETED")){
                System.out.println("File download completed");
            }
        } catch (Exception e) {
            System.out.println(e);
        }
    }

    //Uploading file
    public static void send(ObjectOutputStream os,ObjectInputStream is){
        try {
            System.out.println("Select the file you want to upload");
            FileDialog fileDialog=new FileDialog((Frame)null,"Select file");
            fileDialog.setMode(FileDialog.LOAD);
            fileDialog.setVisible(true);
            System.out.println("Chosen file: "+fileDialog.getDirectory()+ fileDialog.getFile());
            File toUpload=new File(fileDialog.getDirectory()+fileDialog.getFile());

            System.out.println("What name do you want to set to the uploading file?");
            Scanner sc=new Scanner(System.in);
            String name=sc.nextLine();
            os.writeObject(name);

            long fileSize=toUpload.length();
            os.writeObject(fileSize);

            boolean x=(boolean) is.readObject();
            if(!x){
                System.out.println("Buffer limit exceeded");
                return;
            }

            int chunkSize=(int) is.readObject();
            System.out.println("Chunk size: "+chunkSize);
            int fID=(int) is.readObject();
            InputStream fis=new FileInputStream(toUpload);
            byte[] buffer=new byte[chunkSize];

            int len=0;
            socket.setSoTimeout(30000);
            while((len=fis.read(buffer))!=-1){
                //System.out.println(len);
                if(len==chunkSize){
                    os.writeObject(buffer);
                }
                else{
                    byte[] temp=new byte[len];
                    System.arraycopy(buffer,0,temp,0,len);
                    os.writeObject(temp);
                }
                os.reset();
                String ack=(String) is.readObject();
                System.out.println(ack);
                Thread.sleep(1000);
            }
            os.writeObject("COMPLETED");
            fis.close();
            String ck=(String) is.readObject();
            if(ck.equalsIgnoreCase("SUCCESS")){
                System.out.println("File upload successful");
            }
            else if(ck.equalsIgnoreCase("FAILURE")){
                System.out.println("File upload failed");
            }
        }catch(SocketTimeoutException e){
            System.out.println(e);
            try{
                os.writeObject("Timeout");
            }catch (Exception ex){
                System.out.println(ex);
            }
        }
        catch(Exception e){
            System.out.println(e);
        }


    }

    public static void main(String[] args) throws Exception{
        socket=new Socket("localhost",6666);
        System.out.println("Connection established");

        ObjectOutputStream os=new ObjectOutputStream(socket.getOutputStream());
        ObjectInputStream is=new ObjectInputStream(socket.getInputStream());

        Scanner sc=new Scanner(System.in);

        while(true){
            System.out.println("Enter username: ");
            String userName=sc.nextLine();
            os.writeObject(userName);

            boolean login=(boolean) is.readObject();
            if(login){
                System.out.println("Login successful");
                break;
            }
            else {
                System.out.println("User is already logged in");
                is.close();
                os.close();
                socket.close();
                System.exit(0);
            }
        }

        while(true){
            System.out.println("Enter a value between 1-7: ");
            System.out.println("1. Lookup Clients");
            System.out.println("2. Your Files");
            System.out.println("3. Lookup another client's file");
            System.out.println("4. Request for a File");
            System.out.println("5. View unread messages");
            System.out.println("6. Upload a File");
            System.out.println("7. Logout");

            String choice=sc.nextLine();
            os.writeObject(choice);

            if(choice.equals("1")){
                ArrayList<String> active=(ArrayList<String>) is.readObject();
                String[] allUsers=(String[]) is.readObject();
                for(String u:allUsers){
                    if(active.contains(u)){
                        System.out.println(u+": Online");
                    }
                    else
                        System.out.println(u);
                }
            }
            else if(choice.equals("2")){
                ArrayList<String> publicFiles=(ArrayList<String>) is.readObject();

                ArrayList<String> privateFiles=(ArrayList<String>) is.readObject();
                for(String f:publicFiles){
                    System.out.println(f);
                }
                for(String f:privateFiles){
                    System.out.println(f);
                }

                System.out.println("Do you want to download any file?Enter 1/2");
                System.out.println("1.Yes");
                System.out.println("2.No");

                String dChoice=sc.nextLine();
                os.writeObject(dChoice);
                if(dChoice.equals("1")){
                    System.out.println("Enter file ID");
                    String id=sc.nextLine();
                    int fileID;
                    try{
                        fileID= parseInt(id);
                        os.writeObject(fileID);
                    }catch(Exception e){
                        os.writeObject("failed");
                        continue;
                    }
                    String check=(String) is.readObject();
                    if(check.equalsIgnoreCase("exists")){
                        String fileName=(String) is.readObject();
                        receive(fileName,is);
                    }
                    else{
                        System.out.println("There is no file with ID: "+id);
                    }
                }
            }
            else if(choice.equals("3")){
                System.out.println("Mention username of the client: ");

                String userName=sc.nextLine();
                os.writeObject(userName);
                Object o=is.readObject();
                Integer id=0;

                if(o.getClass().equals(id.getClass())){
                    id=(Integer) o;
                    if(id==-1){
                        System.out.println(userName+" is not in the user list");
                        continue;
                    }
                }

                List<String> publicFiles=(List<String>) o;
                System.out.println("Public files of "+userName+": ");
                for(String f:publicFiles){
                    System.out.println(f);
                }

                System.out.println("Do you want to download any file?Enter 1/2");
                System.out.println("1.Yes");
                System.out.println("2.No");

                String dChoice=sc.nextLine();
                os.writeObject(dChoice);
                if(dChoice.equals("1")){
                    System.out.println("Enter file ID");
                    String fileid=sc.nextLine();
                    int fileID;
                    try{
                        fileID= parseInt(fileid);
                        os.writeObject(fileID);
                    }catch(Exception e){
                        os.writeObject("failed");
                        continue;
                    }
                    String check=(String) is.readObject();
                    if(check.equalsIgnoreCase("exists")){
                        String fileName=(String) is.readObject();
                        receive(fileName,is);
                    }
                    else{
                        System.out.println("There is no file with ID: "+fileid);
                    }
                }
            }
            else if(choice.equals("4")){
                System.out.println("Enter description for your request: ");
                String description=sc.nextLine();
                os.writeObject(description);
            }
            else if(choice.equals("5")){

                ArrayList<String> othersRequests=(ArrayList<String>) is.readObject();
                ArrayList<String> ownRequests=(ArrayList<String>) is.readObject();
                System.out.println("File requests: ");
                for(String r:othersRequests){
                    System.out.println("Request ID: "+r);
                }

                System.out.println("Uploads for my requests: ");
                for(String r:ownRequests){
                    System.out.println("Request ID: "+r);
                }
            }
            else if(choice.equals("6")){
                System.out.println("Enter your choice: 1/2/3");
                System.out.println("1. Public");
                System.out.println("2. Private");
                System.out.println("3. Someone's requested file");
                String tp=sc.nextLine();
                os.writeObject(tp);
                if(tp.equals("3")){
                    System.out.println("Enter request ID: ");
                    Integer requestID=parseInt(sc.nextLine());
                    os.writeObject(requestID);

                    boolean ck=(boolean) is.readObject();
                    if(!ck){
                        System.out.println("Invalid request ID");
                        continue;
                    }
                }
                if(tp.equals("1")||tp.equals("2")||tp.equals("3")){
                    send(os,is);
                }
            }
            else if(choice.equals("7")){
                is.close();
                os.close();
                socket.close();
                System.out.println("Logging out");
                System.exit(0);
            }
        }
    }
}
