   Stress tests recommandés                                                                                                       
                                                                                                                                 
  # Siege - modéré (100-500 connexions)                                                                                          
  siege -c 250 -t 30s http://localhost:8080/                                                                                     
                                                                                                                                 
  # Siege - intense (avec keep-alive)                                                                                            
  siege -c 500 -t 30s -b http://localhost:8080/                                                                                  
                                                                                                                                 
  # wrk - meilleur pour haute charge                                                                                             
  wrk -t4 -c1000 -d30s http://localhost:8080/                                                                                    
                                                                                                                                 
  # wrk - très intense                                                                                                           
  wrk -t8 -c4000 -d30s http://localhost:8080/                                                                                    
                                                                                                                                 
  Vérifier les limites de ton système                                                                                            
                                                                                                                                 
  # Limite de file descriptors                                                                                                   
  ulimit -n                                                                                                                      
                                                                                                                                 
  # Augmenter temporairement si besoin                                                                                           
  ulimit -n 65535                                                                                                                
                   
 
 
  # Siege                                                                            
  siege -c 100 -t 30s http://localhost:8080/                                         
                                                                                     
  # Apache Bench                                                                     
  ab -n 1000 -c 100 http://localhost:8080/                                           
                                                                                     
  # wrk                                                                              
  wrk -t12 -c400 -d30s http://localhost:8080/

     pkill -9 webserv 2>/dev/null; echo "Done"                                                                                     

  
  ests basiques                                                                                                                 
                                                                                                                                 
  # GET simple                                                                                                                   
  curl -v http://localhost:8080/cgi-bin/form.py                                                                                  
                                                                                                                                 
  # GET avec query string                                                                                                        
  curl -v "http://localhost:8080/cgi-bin/form.py?name=John&message=Hello"                                                        
                                                                                                                                 
  # POST avec données                                                                                                            
  curl -v -X POST -d "name=Test&message=HelloWorld" http://localhost:8080/cgi-bin/form.py                                        
                                                                                                                                 
  Stress tests CGI                                                                                                               
                                                                                                                                 
  # Siege sur CGI (modéré)                                                                                                       
  siege -c 50 -t 30s http://localhost:8080/cgi-bin/form.py                                                                       
                                                                                                                                 
  # wrk sur CGI                                                                                                                  
  wrk -t4 -c100 -d30s http://localhost:8080/cgi-bin/form.py                                                                      
                                                                                                                                 
  # Apache Bench sur CGI (avec keep-alive)                                                                                       
  ab -n 500 -c 50 -k http://localhost:8080/cgi-bin/form.py                                                                       
                                                                                                                                 
  # POST avec siege                                                                                                              
  siege -c 50 -t 30s "http://localhost:8080/cgi-bin/form.py POST name=test&message=hello"                                        
                                                                                                                                 
  Tests parallèles (CGI + static)                                                                                                
                                                                                                                                 
  # Terminal 1 - CGI                                                                                                             
  wrk -t4 -c100 -d30s http://localhost:8080/cgi-bin/form.py &                                                                    
                                                                                                                                 
  # Terminal 2 - Static (en même temps)                                                                                          
  wrk -t4 -c100 -d30s http://localhost:8080/                                                                                     
                                                                                                                                 
  Test timeout CGI (si tu as un script lent)                                                                                     
                                                                                                                                 
  # Créer un script lent pour tester le timeout                                                                                  
  echo '#!/usr/bin/env python3                                                                                                   
  import time                                                                                                                    
  print("Content-Type: text/plain\n")                                                                                            
  time.sleep(35)  # Plus que le timeout de 30s                                                                                   
  print("Done")' > cgi-bin/slow.py                                                                                               
  chmod +x cgi-bin/slow.py                                                                                                       
                                                                                                                                 
  # Tester                                                                                                                       
  curl -v http://localhost:8080/cgi-bin/slow.py                                                                                  
  # Devrait retourner 504 Gateway Timeout         