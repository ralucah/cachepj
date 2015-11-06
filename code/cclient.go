// go install github.com/ralucah/cclient

package main

import (
    "fmt"
    "log"
    "bytes"
    "strconv"
    "io/ioutil"
    "os"
    "os/exec"
    "strings"
    "crypto/sha1"
    //"runtime"
    "sync"
    "time"
    "net/http"
    "github.com/cockroachdb/cockroach/client"
    //"github.com/cockroachdb/cockroach/util/stop"
)

/* errasure coding parameters */
var k int = 4
var m int = 3
var cockroach_url string = "rpcs://node@localhost:4001"
var db *client.DB

func check(e error) {
    if e != nil {
        log.Fatal(e)
    }
}

func read_file(path string) []byte {
    data, err := ioutil.ReadFile(path)
    check(err)
    //fmt.Print(string(data))
    return data
}

func write_file(path string, data []byte) {
    err := ioutil.WriteFile(path, data, 0644)
    check(err)
}

func file_or_dir(path string) os.FileMode {
    f, err := os.Open(path)
    if err != nil {
        log.Fatal(err)
    }
    defer f.Close()
    fi, err := f.Stat()
    if err != nil {
        log.Fatal(err)
    }

    return fi.Mode()
}

func how_to_use() (string, string) {
    if len(os.Args) != 3 {
        log.Fatal("How to run: ", os.Args[0], " [put|get] [file|dir]")
    }

    cmd := os.Args[1]
    path := os.Args[2]

    if cmd != "put" && cmd != "get" {
        log.Fatal("How to run: ", os.Args[0], " [put|get] [file|dir]")
    }

    return cmd, path
}

func compute_key(path string) string {
    key := sha1.Sum([]byte(path))
    //log.Printf("key: %x\n", key)
    key_str := fmt.Sprintf("%x", key)
    //log.Printf("key_str: %s\n", key_str)
    return key_str
}

func put(path string, chunk int, wg *sync.WaitGroup) {
    fmt.Println(chunk)
    str := []string{path, "_", strconv.Itoa(chunk)}
    chunk_path := strings.Join(str, "")
    key_str := compute_key(chunk_path)
    value := read_file(chunk_path)
    log.Print("put ", chunk_path, " ", key_str)
    err := db.Put(key_str, string(value))
    check(err)
    fmt.Println("put Done")
    wg.Done()
}

func put_chunks(path string) {
    var wg sync.WaitGroup
    wg.Add(k + m)
    for i := 0; i < (k + m); i++ {
        go put(path, i, &wg)
    }
    wg.Wait()
}

func get(path string, chunk int, wg *sync.WaitGroup) {
    str := []string{path, "_", strconv.Itoa(chunk)}
    chunk_path := strings.Join(str, "")
    key_str := compute_key(chunk_path)
    log.Print("get ", chunk_path, " ", key_str)
    res, err := db.Get(key_str)
    check(err)
    write_file(chunk_path, res.ValueBytes())
    fmt.Println("get Done")
    wg.Done()
}

func get_chunks(path string) {
    var wg sync.WaitGroup
    wg.Add(k)
    for i := 0; i < (k + m); i++ {
        go get(path, i, &wg)
    }
    wg.Wait()
}

func encode(path string) {
    cmd := exec.Command("coder", "encode", path, strconv.Itoa(k), strconv.Itoa(m))
    var out bytes.Buffer
    cmd.Stdout = &out
    err := cmd.Run()
    if err != nil {
        log.Fatal(err)
    }
    //fmt.Printf("after run: %q\n", out.String())
}

func decode(path string) {
    cmd := exec.Command("coder", "decode", path, strconv.Itoa(k), strconv.Itoa(m))
    var out bytes.Buffer
    cmd.Stdout = &out
    err := cmd.Run()
    if err != nil {
        log.Fatal(err)
    }
}

func myHandler(writer http.ResponseWriter, req *http.Request) {
    /*err := req.ParseForm()
    if err != nil {
        log.Fatal("Error parsing form!")
    }*/

    fmt.Println("I am the http handler")
    fmt.Println("Method: ", req.Method)
    fmt.Println("URL: ", req.URL)
    fmt.Println("Header: ", req.Header)
    fmt.Println("Body: ", req.Body)
    fmt.Println("ContentLength: ", req.ContentLength)
    fmt.Println("Close: ", req.Close)
    fmt.Println("RemoteAddr: ", req.RemoteAddr)
    fmt.Println("TransferEncoding: ", req.TransferEncoding)

    if req.ContentLength > 0 {
        content, err := ioutil.ReadAll(req.Body)
        if err != nil {
            log.Fatal("Error reading content of http req: ", err)
        }
        fmt.Println("Content: ", string(content))
    }
}

func main() {
    server := &http.Server {
        Addr:           ":8080",
        Handler:        http.HandlerFunc(myHandler),
        ReadTimeout:    10 * time.Second,
        WriteTimeout:   10 * time.Second,
        MaxHeaderBytes: 1 << 20,
    }
    log.Fatal(server.ListenAndServe())

    fmt.Println("After listenAndServe")

    /*log.Println(runtime.NumCPU())

    cmd, path := how_to_use()
    log.Print(cmd, " ", path)

    var err error
    db, err = client.Open(stop.NewStopper(), cockroach_url)
    check(err)

    switch mode := file_or_dir(path); {
    case mode.IsDir():
        fmt.Println("directory")
    case mode.IsRegular():
        fmt.Println("regurlar file")
        switch cmd {
        case "put":
            fmt.Println("put")
            encode(path)
            put_chunks(path)
        case "get":
            fmt.Println("get")
            get_chunks(path)
            decode(path)
        }
    }*/
}
