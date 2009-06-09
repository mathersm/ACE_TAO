// $Id$

// Utility to traverse and simplify verbose log files. 
//
// The goal is to take a collection of log files and extract details
// related to connections, objects, and invocations to separate out
// perhaps multiple processes, or at least threads to get a sense of
// invocation lifecycle.
// 
// Ideally a collection of files could be used so that invocations
// that traverse many processes could be tracked.

#include "ace/OS_NS_stdio.h"
#include "ace/Log_Msg.h"
#include "ace/streams.h"
#include "ace/OS_NS_strings.h"
#include "ace/Tokenizer_T.h"

#include "Invocation.h"
#include "PeerObject.h"
#include "PeerProcess.h"
#include "Session.h"
#include "Log.h"

ACE_TCHAR *outfile = 0;

void
parse_manifest (Session &session, ACE_TCHAR *filename)
{
  // get list of logs, aliases, and other config from file
  ifstream strm (filename);
  if (!strm.is_open())
    {
      ACE_DEBUG ((LM_DEBUG,"cannot open manifest file %C\n", filename));
      return;
    }
  char buffer[500];
  while (!strm.eof())
    {
      strm.getline(buffer,500);
      if (buffer[0] == '\0' || 
          buffer[0] == '#')
        {
          continue;
        }
      if (buffer[0] == '-')
        {
          if (buffer[1] == 'o')
            {
              outfile = new char[ACE_OS::strlen(buffer) - 3];
              ACE_OS::strcpy(buffer+3,outfile);
              continue;
            }
          if (buffer[1] == 't')
            {
              Session::set_tao_version (buffer+3);
              continue;
            }
          if (buffer[1] == 'a')
            {
              session.alternate_address(buffer+3);
              continue;
            }
          if (buffer[1] == 'p')
            {
              session.default_service (buffer+3);
              continue;
            }
        }
      if (ACE_OS::strchr(buffer,'=') == 0)
        {
          ACE_DEBUG ((LM_DEBUG,"Unparsable string: %s\n",
                      buffer));
          return;
        }
      ACE_Tokenizer tokens(buffer);
      tokens.delimiter_replace('=',0);
      char *alias = tokens.next();
      char *filename = tokens.next();
      Log log(session);
      log.init(filename,alias);
    }
}

int
ACE_TMAIN (int argc, ACE_TCHAR **argv)
{
  if (argc < 2)
    {
      ACE_ERROR ((LM_ERROR," At least one log file must be specified\n"));
      return 0;
    }
  Session session;
  for (int i = 1; i < argc; i++)
    {
      if (ACE_OS::strcasecmp (argv[i], "-o") == 0)
        {
          outfile = argv[++i];
          continue;
        }
      if (ACE_OS::strcasecmp (argv[i], "-m") == 0)
        {
          parse_manifest (session, argv[++i]);
          continue;
        }
      if (ACE_OS::strcasecmp (argv[i], "-t") == 0)
        {
          Session::set_tao_version (argv[++i]);
          continue;
        }
      if (ACE_OS::strcasecmp (argv[i], "-a") == 0)
        {
          session.alternate_address (argv[++i]);
          continue;
        }
      if (ACE_OS::strcasecmp (argv[i], "-p") == 0)
        {
          session.default_service (argv[++i]);
          continue;
        }

      Log log(session);
      ACE_DEBUG ((LM_DEBUG,"Parsing file %s\n", argv[i]));
      if (log.init(argv[i]) == 0)
        {
          ACE_ERROR ((LM_ERROR,
                      "Could not parse log file %s\n", argv[i]));
          continue;
        }
      
    }

  session.reconcile();
  ostream *strm;
  if (outfile == 0)
    strm = &cout;
  else
    strm = new ofstream(outfile);

  session.dump(*strm);

  return 0;
}
