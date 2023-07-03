import java.io.File;
import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.SocketAddress;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;
import java.util.Set;

public class Server {
    private static ArrayList<String> users=new ArrayList<>();
    //private static HashMap<String,SocketAddress> students;
    private static HashMap<Integer,String> files;
    private static ArrayList<Request> requests=new ArrayList<>();

    private static final long MAX_BUFFER_SIZE=100000;
    private static final long MAX_CHUNK_SIZE=10000;
    private static final long MIN_CHUNK_SIZE=1000;
    private static long buffer=0;

    private static int reqCount=0;
    private static int fileCount=0;

    public static void main(String[] args) throws Exception {

        String path="FileServer/";
        File dir=new File(path);
        if(!dir.exists()){
            dir.mkdir();
        }

        //students=new HashMap<>();
        files=new HashMap<>();
        ServerSocket fileSocket=new ServerSocket(6666);

        while(true){
            System.out.println("Waiting for connection...");
            Socket socket=fileSocket.accept();
            System.out.println("Connection established");

            Thread worker=new Worker(socket);
            worker.start();
        }
    }

    public static boolean isEligible(long sz){
        if(buffer+sz>MAX_BUFFER_SIZE)
            return false;
        buffer+=sz;
        System.out.println("Buffer occupied: "+buffer+" bytes");
        return true;
    }

    public static void clearBuf(long sz){
        buffer-=sz;
        System.out.println("Buffer occupied: "+buffer+" bytes");
    }

    public static long getMaxChunk(){
        return MAX_CHUNK_SIZE;
    }

    public static int get_random_chunk(){
        int rand= (int) (Math.random()*(MAX_CHUNK_SIZE-MIN_CHUNK_SIZE+1)+MIN_CHUNK_SIZE);
        return rand;
    }

    public static boolean login(String id){
        for(String s:users){
            if(s.equals(id)){
                System.out.println(id + " is already logged in");
                return false;
            }
        }
        /*if(students.containsKey(id)){
            System.out.println(id + " is already logged in");
            return false;
        }*/
        users.add(id);

        //students.put(id,IPaddress);
        System.out.println(id+" has logged in successfully");
        String path="FileServer/"+id+"/";
        File dir=new File(path);

        if(!dir.exists()){
            dir.mkdir();
            File public_dir=new File(path+"public/");
            File private_dir=new File(path+"private/");
            public_dir.mkdir();
            private_dir.mkdir();
            System.out.println("Folder created for "+id+" in File Server");
        }
        return true;
    }

    public static void logout(String id){
        users.remove(id);
        //students.remove(id);
    }

    public static ArrayList<String> active(){
        return users;
        /*Set<String> keys=students.keySet();
        ArrayList<String> activeUsers=new ArrayList<String>(keys);
        return activeUsers;*/
    }

    public static String[] allUsers(){
        File dirpath=new File("FileServer");
        return dirpath.list();
    }

    public static void addRequest(String id,String description){
        requests.add(new Request(reqCount,id,description));
        reqCount++;
    }

    public static ArrayList<Request> getRequests(){
        return requests;
    }

    public static void addUpload(int reqID,String granter,String filePath){
        for(Request r:requests){
            if(r.getID()==reqID){
                r.acceptRequest(granter,filePath);
                //System.out.println("here");
                break;
            }
        }
    }

    public static void removeRequest(int reqID){
        for(int i=0;i<requests.size();i++){
            if(requests.get(i).getID()==reqID){
                requests.remove(i);
                break;
            }
        }
        //System.out.println("here2");
    }

    public static boolean validRequestID(int reqID){
        for(Request r:requests){
            if(r.getID()==reqID){
                return true;
            }
        }
        return false;
    }

    public static int addFile(String filepath){
        fileCount++;
        files.put(fileCount,filepath);
        return fileCount;
    }

    public static String getFilePath(int fileID){
        return files.get(fileID);
    }

    public static Integer getFileID(String filepath){
        for(Map.Entry<Integer,String>entry: files.entrySet()){
            if(entry.getValue().equals(filepath)){
                return entry.getKey();
            }
        }
        return 0;
    }
}
