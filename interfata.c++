using System.Net.Sockets;
using System;

411    void
412    main(int argc, char * argv[])
413    {
    414      int port = 8082;
    415      int i = 0;
    416      char* pszLog = 0;
    417      char szExit[20];
    418      snorkel_obj_t http = 0, logobj = 0;
    419
420      fprintf(stderr,
421               "\nnetstat server -- a remote port monitoring server\n\n");
    422
423      for (i = 1; i < argc; i++)
        424        {
        425          if (argv[i][0] == '-')
            426            {
            427              switch (argv[i][1])
428                {
                429                case 'l':
430                  if (i + 1 >= argc)
                    431                    syntax(argv[0]);
                432                  pszLog = argv[i + 1];
                433                  i++;
                434                  break;
                435                case 'r':
436                  if (i + 1 >= argc)
                    437                    syntax(argv[0]);
                438                  g_autoRefreshInSeconds =
                439                    atoi(argv[i + 1]);
                440                  i++;
                441                  break;
                442                default:
443                  syntax(argv[0]);
                444                  break;
                445                }
            446            }
        447          else
            448            {
            449              port = atoi(argv[i]);
            450              break;
            451            }
        452        }
    453
454
455      if (pszLog)
        456        {
        457          logobj =
        458            snorkel_obj_create(snorkel_obj_log,
        459                                pszLog);
        460          if (!logobj)
            461            {
            462              perror("could not create log file\n");
            463              exit(1);
            464            }
        465          snorkel_debug(1);
        466        }
    467
468      if (snorkel_init() != SNORKEL_SUCCESS)
        469        {
        470          perror("could not initialize snorkel\n");
        471          exit(1);
        472        }
    473
474      http =
475        snorkel_obj_create(snorkel_obj_server, 2,
476                            NULL);
    477      if (!http)
        478        {
        479          perror
        480("could not create server object!\n");
        481          exit(1);
        482        }
    483
484      if (snorkel_obj_set(http,    /* server object */
485                           snorkel_attrib_listener, /* attribute   */
486                           port,    /* port number */
487                           0 /* SSL support */ )
488 != SNORKEL_SUCCESS)
489        {
        490          fprintf(stderr,
        491                   "could not create listener\n");
        492          snorkel_obj_destroy(http);
        493          exit(1);
        494        }
    495
496      /*
497       *
498       * overload the URI http://index.html
499       *
500       */
501      if (snorkel_obj_set(http,    /* server object */
502                           snorkel_attrib_uri,      /* attribute type */
503                           GET,     /* method */
504                           "/index.html",   /* uri */
505                           encodingtype_text,       /* encoding */
506                           MainPage) !=
507          SNORKEL_SUCCESS)
508        {
        509          perror("could not overload index.html");
        510          snorkel_obj_destroy(http);
        511          exit(1);
        512        }
    513
514      if (snorkel_obj_set
515(http, snorkel_attrib_ipvers, IPVERS_IPV4,
516           SOCK_SET) != SNORKEL_SUCCESS)
517        {
        518          fprintf(stderr,
        519                   "error could not set ip version\n");
        520          exit(1);
        521        }
    522
523      /*
524       *
525       * start the server
526       *
527       */
528      fprintf(stderr,
529               "\n\n[HTTP] starting embedded server\n");
    530      if (snorkel_obj_start(http) != SNORKEL_SUCCESS)
        531        {
        532          perror("could not start server\n");
        533          snorkel_obj_destroy(http);
        534          exit(1);
        535        }
    536
537      /*
538       *
539       * do something while server runs
540       * as a separate thread
541       *
542       */
543      fprintf(stderr, "\n[HTTP] started.\n\n"
544               "--hit enter to terminate--\n");
    545      fgets(szExit, sizeof(szExit), stdin);
    546
547      fprintf(stderr, "[HTTP] bye\n");
    548
549      /*
550       *
551       * graceful clean up
552       *
553       */
554      snorkel_obj_destroy(http);
    555      exit(0);
    556    }