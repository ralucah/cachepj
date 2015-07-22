from __future__ import with_statement
from fabric.api import *
from fabric.colors import green, red, blue
from fabric.contrib.files import exists

# hosts
env.roledefs = {
    'master_host' : ['172.16.0.27'],
    'server_hosts': ['172.16.0.28', '172.16.0.29', '172.16.0.30'],
    'client_hosts': ['172.16.0.31']
}

maddr = env.roledefs['master_host'][0]
work_dir = '/home/ubuntu/epaxos'

# commands + configurations
cmd_master = "bin/master -N=%d" % len(env.roledefs['server_hosts']) # default port 7087
cmd_server = "bin/server -addr='%s' -beacon=false -dreply=true -durable=false -e=true -exec=true -maddr='%s' -p=2 -thrifty=false" # default port 7070
cmd_client = "bin/client -c=-1 -check=false -e=true -eps=0 -maddr='%s' -p=2 -q=5000 -r=1 -s=2 -v=1 -w=100"

# env settings
env.colorize_errors = True
env.eagerly_disconnect = True

# start master in background (nonblocking)
@task(alias = 'master')
@roles('master_host')
def start_master():
    execute(clean_master)
    print blue("Master %s" % env.host)
    with cd(work_dir):
         run_bg(cmd_master)

# start server (blocking)
@task(alias = 'server')
@roles('server_hosts')
@parallel
def start_server():
    print blue("Server %s with Master %s" % (env.host, maddr))
    with cd(work_dir):
        run(cmd_server % (env.host, maddr))

# start a client per host
@task(alias = 'client')
@roles('client_hosts')
@parallel
def start_client():
    print blue("Client %s with  Master %s" % (env.host, maddr))
    with cd(work_dir):
        run(cmd_client % maddr)

# start x clients per host
@task(alias = 'clientx')
@roles('client_hosts')
@parallel
def start_clientx(x = 5):
    if not exists("/usr/bin/xargs"):
        sudo("apt-get install xargs")
    print blue("Client %s with  Master %s" % (env.host, maddr))
    with cd(work_dir):
        cmd = "echo {1..%s} | xargs -P %s -n 1 -d ' ' -I* " + cmd_client
        run(cmd  % (x, x, maddr))


# clean master and servers
@task
@parallel
def clean():
    execute(clean_master)
    execute(clean_server)    

# run command in background
def run_bg(cmd, sockname="dtach"):
    return run('dtach -n `mktemp -u /tmp/%s.XXXX` %s'  % (sockname,cmd))

# clean master
@roles('master_host')
def clean_master():
    with settings(warn_only = True), hide('everything'):
        result = run("killall master")
    if result.failed:
        print red("Could not kill master %s!" % env.host)
    else:
        print green("Killed master %s!" % env.host)

# clean server
@roles('server_hosts')
def clean_server():
    with settings(warn_only = True), hide('everything'):
        result = run("killall server")
    if result.failed:
        print red("Could not kill server %s!" % env.host)
    else:
        print green("Killed server %s!" % env.host)
        
