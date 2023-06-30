import java.io.*;
import java.net.Socket;
import java.net.SocketAddress;

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

    public void sendFile(File file, ObjectInputStream in, ObjectOutputStream out){
        try{
            out.writeObject(file.length()); //file size
            int chunkSize=Server.get_random_chunk();
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
            os.writeObject(fileID);
            File file=new File(curFile);

            FileOutputStream fos=new FileOutputStream(file);
            BufferedOutputStream bos=new BufferedOutputStream(fos);
            curStream=fos;

            String acknowledge="";
            boolean terminate=false;
            int bytesread=0;
            int total=0;
            long remainder=fileSize%chunkSize;
            long iteration=fileSize/chunkSize;
            if(remainder!=0)
                iteration++;
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
}
