//
//  main.cpp
//  P1 Operating System

#include <iostream>
#include <fstream>
#include <unistd.h>
#include <getopt.h>
#include <fcntl.h> // for open


using namespace std;

int main(int argc, char * argv[])
{
    void PrintUsage();

    bool HandleOptions(int argc, char ** argv, string &, string &, string &, string &, string &, string &, string &);

    string t;
    string d;//prints the directory 
    string i; //file path string
    string o;//this option deletes everything on that file
    string a; //this adds thingd to the file hence the name append 
    string one; //this option is mandatory 
    string two; //this option is for piping from 1 
    string D; //d is supposed to sho the current directory 
    int status;
    const int32_t READSIDE = 0; 
    const int32_t WRITESIDE = 1;
    int pipefd[2];

    if (!HandleOptions(argc, argv, t, i, o, a, one, two, D)) 
    {
		PrintUsage();
		return 1;
	}

    pid_t child_pid = fork(); //makes a baby

    if (child_pid == -1) 
    {
        perror("fork");
        return 1;
    }//check to see if baby alright

    if (pipe(pipefd) == -1) 
    {
        perror("pipe");
        return 1;
    }

    if(child_pid == 0) //we enter the childs program 
    {

        //implementing -2------------------------------------
     if (!two.empty())//I check to see if 2 is being used 
     {
        if (pipe(pipefd) == -1) //I make sure the pipe is not error
        {
         perror("pipe");
         return 1;
        }

        // Fork a child2 process (parent process)
        pid_t child_pid2 = fork(); //I then create the second child within the other child 

        if (child_pid2 == -1) //check to see if its not an error
        {
         perror("fork");
         return 1;
        }

        if (child_pid2 == 0) //I are now inside the second child program 
        {
         close(pipefd[WRITESIDE]); // I then close the write end of the pipe
         dup2(pipefd[READSIDE], STDIN_FILENO); // Then i redirect stdin to the read end of the pipe
         close(pipefd[READSIDE]); // Then i close the read end of the pipe

         const char* command2 = const_cast<char*>(two.c_str());//then the second command -2
         char* argument_list[] = {(char*)command2,nullptr}; //Then I put it in the char* list 
         execvp(command2, argument_list);
         perror("execvp"); // execvp failed
         return 1;//return 1 if failed
        }
        else if (child_pid2 > 0)//parent for chidl 2
        {
         close(pipefd[READSIDE]); //I finish so we close the READSIDE
         close(pipefd[WRITESIDE]); //I finsish so we close the WRITESIDE
         waitpid(child_pid2, &status, 0); //then we use wait for the parent to wait for the child to be done and be able to close child 2
         cout <<"Child 2: " << child_pid2 << " retruns: " << WEXITSTATUS(status) <<  endl;
        }
    
     
     }//option 2 close bracket 


         /*Implemneting o optiong*/
        if (!o.empty())
        {
         int output_fd = open(o.c_str(), O_WRONLY | O_CREAT | O_TRUNC);
         if (output_fd == -1) 
         {
          perror("open");
          return 1;
         }
            dup2(output_fd, STDOUT_FILENO); //I then Redirect stdout to the output file
            close(output_fd);
        }

        /*Implemneting a optiong*/
        if (!a.empty())
        {
            int fd = open(a.c_str(), O_WRONLY | O_CREAT | O_APPEND);
            if (fd == -1)
            {
                perror("open");
                return 1;
            }

            // I am going to redirect stdout to the file
            close(STDOUT_FILENO);
            dup2(fd, STDOUT_FILENO);

            //Then I close the file descriptor for the file || (it's duplicated in stdout now)
            close(fd);
        }

        /*Implenting the t option */
        if (!t.empty()) 
        {
         const char* command = const_cast<char*>(t.c_str());
         if (chdir(command) != 0) 
         {
          cerr << "Error: Failed to change directory to " << t << endl;
          return 1;
         }
         char* argument_list[] = {(char*)command, nullptr, nullptr};
         execvp(command, argument_list);

        }

        /*Implenting the i option*/
        if (!i.empty()) 
        {
         // I try to open the input file
         const char* command = const_cast<char*>(one.c_str());
         const char* command2 = const_cast<char*>(i.c_str());
         char* argument_list[] = {(char*)command,(char*)command2,nullptr};
         execvp(command, argument_list);
    
        }

        const char* command = const_cast<char*>(one.c_str());
        char* argument_list[] = {(char*)command,nullptr,nullptr};
        
        execvp(command, argument_list);

       
    }
    else if (child_pid > 0)// this is for the parent 
    {
        waitpid(child_pid, &status, 0);//this catches the first kid
        cout <<"Child 1: " << child_pid << " retruns: " << WEXITSTATUS(status) <<  endl;
    
    }
   
    return 0;
}

void PrintUsage()
{
    cerr << "-1    followed by a path of a program to run." << endl;
    //-v this option is optional for debugging 
}

bool HandleOptions(int argc, char ** argv, string & t,string & i, string & o, string & a, string & one, string & two, string & D)
{
    int c;
    bool hasOne = false;
    

        while ((c = getopt(argc, argv, "ht:i:o:a:1:D2:")) != -1) {
            switch (c) 
            {
                default:
                case 'h':
                    // Short circut forcing the usage to be printed by returning false from this function.
                    return false;

                case 't':
                    t = string(optarg); //done
                    break;
                    
                case 'i':
                    i = string(optarg);//done
                    break;

                case 'o':
                    o = string(optarg);
                    break;

                case 'a':
                    a = string(optarg);
                    break;

                case '1':
                    one = string(optarg); //done
                    hasOne = true;
                    break;

                case 'D':
                    char temp[256]; //done
                    cout << getcwd(temp, sizeof(temp)) << endl;
                    break;

                case '2':
                    two = string(optarg);
                    break;
                }
            }

    return hasOne;
}
