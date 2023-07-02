import java.io.*;
import java.net.Socket;
import java.net.SocketAddress;
import java.util.ArrayList;
import java.util.List;

public class Worker extends Thread{
    Socket socket;
    SocketAddress clientIP;
    String ID;
    boolean active=true;
    String curFile=null;
    FileOutputStream curStream=null;
    long fileSize=0;

    public Worker(Socket socket)
    {
        this.socket=socket;
        clientIP= socket.getRemoteSocketAddress();
    }

    //downloading from server
    public void sendFile(File file, ObjectInputStream in, ObjectOutputStream out){
        try{
            out.writeObject(file.length()); //file size
            int chunkSize= (int) Server.getMaxChunk();
            InputStream is=new FileInputStream(file); //creating input stream from the file
            byte[] buffer=new byte[chunkSize]; //create buffer
            int len=0;
            while((len=is.read(buffer))!=-1){
                if(len==chunkSize){
                    out.writeObject(buffer);
                }
                else{
                    //If len < chunksize
                    byte[] buffer2=new byte[len];
                    System.arraycopy(buffer,0,buffer2,0,len);
                    out.writeObject(buffer2);
                }
                out.reset();
            }
            out.writeObject("COMPLETED");
            is.close();
        }
        catch (Exception e){
            System.out.println(e);
        }
    }

    //uploading to server
    public String receiveFile(String type,ObjectInputStream is,ObjectOutputStream os){
        try{
            String filename=(String) is.readObject();
            fileSize=(long) is.readObject();
            System.out.println(ID+" wants to upload a "+type+" file "+filename+" of size: "+fileSize+" bytes");
            boolean eligible=Server.isEligible(fileSize);
            os.writeObject(eligible);
            if(!eligible)
                return null;
            int chunkSize=Server.get_random_chunk();
            os.writeObject(chunkSize);
            curFile="FileServer/"+ID+"/"+type+"/"+filename;
            int fileID=Server.addFile(curFile);
            System.out.println(fileID);
            os.writeObject(fileID);
            File file=new File(curFile);

            FileOutputStream fos=new FileOutputStream(file);
            BufferedOutputStream bos=new BufferedOutputStream(fos);
            curStream=fos;

            String acknowledge="";
            boolean terminate=false;
            int bytesread=0;
            int total=0;
            long remainder=( ( (fileSize % chunkSize) - 1 ) >> 31) ^ 1;
            long iteration=(fileSize / chunkSize) + remainder;
            //if(remainder!=0)
                //iteration++;
            for(long i=0;i<iteration;i++){
                Object o=is.readObject();
                if(o.getClass().equals(acknowledge.getClass())){
                    acknowledge=(String) o;
                    terminate=true;
                    break;
                }
                byte[] con=(byte[]) o;
                bytesread=con.length;
                total+=bytesread;
                bos.write(con,0,bytesread);
                os.writeObject(total+" bytes received out of "+fileSize);
            }
            bos.flush();
            bos.close();
            fos.close();
            curStream=null;

            if(!terminate){
                acknowledge=(String) is.readObject();
            }

            System.out.println(acknowledge);
            if(acknowledge.equalsIgnoreCase("COMPLETED")){
                Server.clearBuf(total);
                if(total==fileSize){
                    os.writeObject("SUCCESS");
                }
                else {
                    System.out.println(curFile+" deleting due to incomplete");
                    os.writeObject("FAILURE");
                    File toBeDeleted=new File(curFile);
                    boolean check=toBeDeleted.delete();
                    if(check){
                        System.out.println(curFile+" deletion successful");
                    }
                    else{
                        System.out.println(curFile+" deletion failed");
                    }
                }
            }
            else if(acknowledge.equalsIgnoreCase("TIMEOUT")){
                File toBeDeleted=new File(curFile);
                boolean check=toBeDeleted.delete();
                if(check){
                    System.out.println(curFile+" deletion successful");
                }
                else{
                    System.out.println(curFile+" deletion failed");
                }
            }
            curFile=null;
            return filename;
        }
        catch(Exception e){
            System.out.println(e);
            return null;
        }
    }

    public void run()
    {
        try{
            ObjectOutputStream os=new ObjectOutputStream(this.socket.getOutputStream());
            ObjectInputStream is=new ObjectInputStream(this.socket.getInputStream());

            while(active){
                ID=(String) is.readObject();
                boolean in=Server.login(ID,clientIP);
                os.writeObject(in);
                if(!in)
                    continue;

                while(true){
                    String choice=(String) is.readObject();

                    if(choice.equals("1")){
                        //Showing user list
                        System.out.println(ID+" requested for user list");
                        ArrayList<String> activeUsers=Server.active();
                        os.writeObject(activeUsers);
                        String[] all=Server.allUsers();
                        os.writeObject(all);
                    }
                    else if(choice.equals("2")){
                        //Showing own files and downloading them (if required)
                        List<String> publicfiles=new ArrayList<>();
                        List<String> privatefiles=new ArrayList<>();
                        publicfiles.add("Public: ");
                        File[] publicFiles=new File("FileServer/"+ID+"/public/").listFiles();

                        for(File f:publicFiles){
                            if(f.isFile()){
                                int fileID=Server.getFileID("FileServer/"+ID+"/public/"+f.getName());
                                publicfiles.add(fileID+": "+f.getName());
                            }
                        }
                        //os.writeObject(files);
                        //files.clear();

                        privatefiles.add("Private: ");
                        File[] privateFiles=new File("FileServer/"+ID+"/private/").listFiles();

                        for(File f:privateFiles){
                            if(f.isFile()){
                                int fileID=Server.getFileID("FileServer/"+ID+"/private/"+f.getName());
                                privatefiles.add(fileID+": "+f.getName());
                            }
                        }

                        os.writeObject(publicfiles);
                        os.writeObject(privatefiles);
                        //files.clear();

                        //Want to download or not
                        String download=(String) is.readObject();
                        if(download.equals("1")){
                            int fileID;
                            try {
                                fileID=(int) is.readObject();
                            }
                            catch(Exception e){
                                continue;
                            }
                            System.out.println(fileID);
                            String path=Server.getFilePath(fileID);
                            System.out.println(ID +" wants to download "+path);
                            if(path==null){
                                System.out.println("No such file in your directory");
                                os.writeObject("No such file in your directory");
                                continue;
                            }
                            if(path.startsWith("FileServer/" + ID+ "/public/") || path.startsWith("FileServer/" + ID + "/private/")){
                                os.writeObject("exists");
                                File file=new File(path);
                                os.writeObject(file.getName());
                                sendFile(file,is,os);
                            }
                            else{
                                System.out.println("No such file in your directory");
                                os.writeObject("No such file in your directory");
                            }
                        }
                    }
                    else if(choice.equals("3")){
                        String id=(String) is.readObject();
                        String[] allUsers=Server.allUsers();
                        int p=0;
                        for(String u:allUsers){
                            if(u.equals(id)){
                                p=1;
                                break;
                            }
                        }
                        if(p==1){
                            System.out.println(ID+" wants to see the files of "+id);
                            List<String> files=new ArrayList<>();
                            File[] publicFiles=new File("FileServer/"+id+"/public/").listFiles();

                            for(File f:publicFiles){
                                if(f.isFile()){
                                    int fileID=Server.getFileID("FileServer/"+id+"/public/"+f.getName());
                                    files.add(fileID+": "+f.getName());
                                }
                            }

                            os.writeObject(files);

                            //Download or not
                            String download=(String) is.readObject();
                            if(download.equals("1")){
                                /*String filename=(String) is.readObject();
                                File file=new File("FileServer/"+id+"/public/"+filename);*/

                                int fileID;
                                try {
                                    fileID=(int) is.readObject();
                                }
                                catch(Exception e){
                                    continue;
                                }
                                System.out.println(fileID);
                                String path=Server.getFilePath(fileID);
                                System.out.println(ID +" wants to download "+path);
                                if(path==null){
                                    System.out.println("No such file");
                                    os.writeObject("No such file");
                                    continue;
                                }
                                if(path.startsWith("FileServer/" + id+ "/public/")){
                                    os.writeObject("exists");
                                    File file=new File(path);
                                    os.writeObject(file.getName());
                                    sendFile(file,is,os);
                                }
                                else{
                                    System.out.println("No such file");
                                    os.writeObject("No such file");
                                }
                            }
                        }
                        else{
                            System.out.println(id+" does not exist");
                            os.writeObject(-1);
                        }
                    }
                    else if(choice.equals("4")){
                        System.out.println(ID+" wants to request for a file");
                        String description=(String) is.readObject();
                        Server.addRequest(ID,description);
                    }
                    else if(choice.equals("5")){
                        ArrayList<Request> requests=Server.getRequests();
                        ArrayList<String> ownRequests=new ArrayList<>();
                        ArrayList<String> othersRequests=new ArrayList<>();
                        for(Request r: requests){
                            if(r.getRequester().equals(ID)){
                                ArrayList<UploadInfo> uploads=new ArrayList<>();
                                uploads=r.getUploads();
                                //System.out.println(uploads.isEmpty());
                                if(!(uploads.isEmpty())){
                                    for(UploadInfo u:uploads){
                                        ownRequests.add(r.getID()+": "+u.getUploader()+" uploaded: "+u.getFilepath());
                                    }
                                }
                                //Server.removeRequest(r.getID());
                            }
                            else{
                                othersRequests.add(r.getID()+": "+r.getRequester()+" requested for a file: "+r.getDescription());
                            }
                        }
                        //System.out.println("hello");
                        os.writeObject(othersRequests);
                        os.writeObject(ownRequests);
                    }
                    else if(choice.equals("6")){
                        String type=(String) is.readObject();
                        int reqID=-1;
                        if(type.equals("1")){
                            type="public";
                        }
                        else if(type.equals("2")){
                            type="private";
                        }
                        else if(type.equals("3")){
                            type="public";
                            reqID=(int) is.readObject();
                            boolean exist=Server.validRequestID(reqID);
                            os.writeObject(exist);
                            if(!exist)
                                continue;
                        }
                        String name=receiveFile(type,is,os);
                        Server.addUpload(reqID,ID,"FileServer/"+ID+"/public/"+name);
                    }
                    else if(choice.equals("7")){
                        is.close();
                        os.close();
                        socket.close();
                        Server.logout(ID);
                        System.out.println(ID+" has logged out");
                        active=false;
                        break;
                    }
                }
            }
        }catch(Exception e) {
            if (curFile != null) {
                try {
                    if (curStream != null) {
                        curStream.close();
                    }
                } catch (IOException ex) {
                    System.out.println(ex);
                }

                File toBeDeleted = new File(curFile);
                Server.clearBuf(fileSize);
                boolean ok = toBeDeleted.delete();
                if (ok) {
                    System.out.println(curFile + " deleting due to " + ID + " going offline");
                } else {
                    System.out.println(curFile + " deleting failed");
                }
                curFile = null;
            }
            System.out.println(ID + " has gone offline, logging out...");
            Server.logout(ID);
        }
    }
}
