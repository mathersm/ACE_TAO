#$Id$
# -*- perl -*-
eval '(exit $?0)' && eval 'exec perl -S $0 ${1+"$@"}'
    & eval 'exec perl -S $0 $argv:q'
    if 0;

use lib "../../../../bin";

require ACEutils;

# setup CosEC params such that..
# cos event service name = "cosec1"
# for ConsumerQOS: EventID = 21, SourceID = 6
# for SupplierQOS: EventID = 20, SourceID = 5.
$CosEC1_params = "-n cosec1 -e 21 -o 6 -p \"5 20\"";

# setup another CosEC params such that..
# cos event service name = "cosec2"
# for ConsumerQOS: EventID = 20, SourceID = 5
# for SupplierQOS: EventID = 21, SourceID = 6.
$CosEC2_params = "-n cosec2 -e 20 -o 5 -p \"6 21\"";

sub cosec_multiple_test1
{
    # first start the Naming service..
    $SV1 = Process::Create ($EXEPREFIX."../../Naming_Service/Naming_Service".$Process::EXE_EXT,"");

    sleep 10;

    # now start the Rt EC..
    $SV2 = Process::Create ($EXEPREFIX."../../Event_Service/Event_Service".$Process::EXE_EXT,"");

    sleep 10;

    # now start the CosEC1..
    $SV3 = Process::Create ($EXEPREFIX."../../CosEvent_Service/CosEvent_Service".$Process::EXE_EXT,$CosEC1_params);

    sleep 10;

    # now start the CosEC2..
    $SV4 = Process::Create ($EXEPREFIX."../../CosEvent_Service/CosEvent_Service".$Process::EXE_EXT,$CosEC2_params);

    sleep 10;

    #start 1 consumer that uses CosEC1 to receive 2 events
    $CONS = Process::Create ($EXEPREFIX."consumer".$Process::EXE_EXT,"-n cosec1 -c 2");

    sleep 10;

    #start 1 supplier  that uses CosEC2 to send 2 events
    $SUPP = Process::Create ($EXEPREFIX."supplier".$Process::EXE_EXT,"-n cosec2 -c 2");

    sleep 10;

    #wait for the supplier to finish
    $SUPP->Wait ();


    #wait for the consumer to finish
    $CONS->Wait ();

    # cleanup..
    $SV1->Kill ();
    $SV2->Kill ();
    $SV3->Kill ();
    $SV4->Kill ();

    $SV1->Wait ();
    $SV2->Wait ();
    $SV3->Wait ();
    $SV4->Wait ();
}

cosec_multiple_test1 ();
