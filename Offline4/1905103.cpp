#include<bits/stdc++.h>
#include<windows.h>
using namespace std;

int main()
{
    string data;
    int m;
    double p;
    string gen_polynomial;

    cout<<"enter data string: ";
    getline(cin,data);
    cout<<"enter number of data bytes in a row <m>: ";
    cin>>m;
    cout<<"enter probability <p>: ";
    cin>>p;
    cin.get();
    cout<<"enter generator polynomial: ";
    cin>>gen_polynomial;

    //padding until the size of the data string is a multiple of m
    while(data.size()%m!=0)
    {
        data+='~';
    }

    cout<<endl<<"data string after padding: "<<data<<endl;

    //creating data block
    vector<string> data_block;
    for(int i=0;i<data.size();i++)
    {
        string binary_ascii_value=bitset<8>((int)data[i]).to_string();
        if(i%m==0)
            data_block.push_back(binary_ascii_value);
        else
            data_block[i/m].append(binary_ascii_value);
    }

    cout<<endl<<"data block <ascii code of m characters per row>:"<<endl;
    for(int i=0;i<data.size()/m;i++)
        cout<<data_block[i]<<endl;

    //creating data block after adding check bits
    vector<int> pr;
    int r,x=1;
    //pr.push_back(1);
    for(int i=0;;i++)
    {
        if((8*m)+1<=x-i)
        {
            r=i;
            break;
        }
        pr.push_back(x);
        x*=2;
    }
    //cout<<x<<endl;
    vector<string> data_block_with_check_bits;
    for(int i=0;i<data.size()/m;i++)
    {
        int pos=-1,rc=0;
        for(int j=0;j<(8*m)+r;j++)
        {
            if(rc<r)
            {
                if(j+1==pr[rc])
                {
                    //cout<<"here"<<endl;
                    if(j==0)
                        data_block_with_check_bits.push_back("0");
                    else
                        data_block_with_check_bits[i]+='0';
                    //data_block_with_check_bits[i]+='0'; //Initially parity bits are assigned 0
                    rc++;
                }
                else
                {
                    data_block_with_check_bits[i]+=data_block[i][pos+1];
                    pos++;
                }
            }
            else
            {
                data_block_with_check_bits[i]+=data_block[i][pos+1];
                pos++;
            }
            //cout<<data_block_with_check_bits[i]<<endl;
        }

        //cout<<"here"<<endl;

        for(int j=0;j<r;j++)
        {
            int val=0;
            for(int k=0;k<(8*m)+r;k++)
            {
                if((k+1)&(1<<j))
                {
                    // int tmp=data_block_with_check_bits[i][pr[j]-1]-'0';
                    val^=(data_block_with_check_bits[i][k]-'0');
                }
            }
            data_block_with_check_bits[i][pr[j]-1]=(char)('0'+val);
        }
    }

    HANDLE h = GetStdHandle ( STD_OUTPUT_HANDLE );
    WORD word_old_color_attributes;
    CONSOLE_SCREEN_BUFFER_INFO console_screen_buffer_info;
    GetConsoleScreenBufferInfo(h, &console_screen_buffer_info);
    word_old_color_attributes = console_screen_buffer_info.wAttributes;

    cout<<endl<<"data block after adding check bits:"<<endl;
    for(int i=0;i<data.size()/m;i++)
    {
        int x=1;
        for(int j=0;j<(8*m)+r;j++)
        {
            if(j+1==x)
            {
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
                cout<<data_block_with_check_bits[i][j];
                x*=2;
            }
            else
            {
                SetConsoleTextAttribute ( h, word_old_color_attributes);
                cout<<data_block_with_check_bits[i][j];
            }
        }
        cout<<endl;
    }

    //Serializing in column major manner
    string sender_column_serialized;
    for(int i=0;i<(8*m)+r;i++)
    {
        for(int j=0;j<data.size()/m;j++)
        {
            sender_column_serialized.push_back(data_block_with_check_bits[j][i]);
        }
    }
    cout<<endl<<"data bits after column-wise serialization:"<<endl;
    cout<<sender_column_serialized<<endl;

    //calculating CRC checksum for sender frame
    string tmp=sender_column_serialized;
    int degree,gen_len=gen_polynomial.size();
    for(int i=0;i<gen_len;i++)
    {
        if(gen_polynomial[gen_len-1-i]=='1')
            degree=i;
    }
    for(int i=0;i<degree;i++)
        sender_column_serialized.push_back('0');
    
    for(int i=0;i<=sender_column_serialized.size()-degree-1;i++)
    {
        if(sender_column_serialized[i]=='0')
            continue;
        for(int j=0;j<=degree;j++)
        {
            if(sender_column_serialized[i+j]==gen_polynomial[j])
                sender_column_serialized[i+j]='0';
            else
                sender_column_serialized[i+j]='1';
        }
    }

    string remainder;
    for(int i=degree;i>0;i--)
        remainder.push_back(sender_column_serialized[sender_column_serialized.size()-i]);
    sender_column_serialized=tmp;
    sender_column_serialized.append(remainder);

    cout<<endl<<"data bits after appending CRC checksum (sent frame):"<<endl;
    for(int i=0;i<sender_column_serialized.size();i++)
    {
        if(i>=sender_column_serialized.size()-degree)
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 3);
        cout<<sender_column_serialized[i];
    }
    cout<<endl;
    SetConsoleTextAttribute(h,word_old_color_attributes);

    //Simulating received frame
    int frame_size=sender_column_serialized.size();

    vector<int> pos;

    srand(time(0));

    for(int i=0;i<frame_size;i++)
    {
        int random_number=rand()%10000;
        if(random_number*0.0001<p)
            pos.push_back(i);
    }

    for(int i=0;i<pos.size();i++)
    {
        if(sender_column_serialized[pos[i]]=='0')
            sender_column_serialized[pos[i]]='1';
        else
            sender_column_serialized[pos[i]]='0';
    }

    cout<<endl<<"received frame:"<<endl;
    int idx=-1;
    for(int i=0;i<frame_size;i++)
    {
        if(idx+1<pos.size())
        {
            if(i==pos[idx+1])
            {
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),4);
                idx++;
            }
            else
                SetConsoleTextAttribute(h,word_old_color_attributes);
        }
        else
            SetConsoleTextAttribute(h,word_old_color_attributes);
        cout<<sender_column_serialized[i];
    }
    cout<<endl;

    int flag=0;
    tmp=sender_column_serialized;
    for(int i=0;i<frame_size-degree;i++)
    {
        if(sender_column_serialized[i]=='0')
            continue;
        for(int j=0;j<=degree;j++)
        {
            if(sender_column_serialized[i+j]==gen_polynomial[j])
                sender_column_serialized[i+j]='0';
            else
                sender_column_serialized[i+j]='1';
        }
    }
    for(int i=0;i<frame_size;i++)
    {
        if(sender_column_serialized[i]=='1')
        {
            flag=1;
            break;
        }
    }

    cout<<endl<<"result of CRC checksum matching: ";
    if(flag)
        cout<<"error detected"<<endl;
    else
        cout<<"no error detected"<<endl;
}
