package main

import (
	"encoding/csv"
	"errors"
	"flag"
	"fmt"
	"io"
	"os"
)

type inputFile struct {
	filepath  string
	separator string
	pretty    bool
}

func getFileData() (inputFile, error) {
	if len(os.Args) < 2 {
		return inputFile{}, errors.New("A filepath is required!")
	}

	separator := flag.String("separator", "comma", "Column separator")
	pretty := flag.Bool("pretty", false, "Generate pretty JSON")

	flag.Parse()

	fileLocation := flag.Arg(0)

	if !(*separator == "comma" || *separator == "semicolon") {
		return inputFile{}, errors.New("Only comma or semicolon separators are allowed")
	}

	return inputFile{fileLocation, *separator, *pretty}, nil
}

func processCsvFile(fileData inputFile, writerChannel chan<- map[string]string) {
	file, err := os.Open(fileData.filepath)
	check(err)

	defer file.Close()

	var headers []string

	reader := csv.NewReader(file)

	if fileData.separator == "semicolon" {
		reader.Comma = ';'
	}

	headers, err = reader.Read()
	check(err)

	for {
		line, err := reader.Read()

		if err == io.EOF {
			close(writerChannel)
			break
		} else if err != nil {
			exitGracefully(err)
		}

		record, err := processLine(headers, line)

		if err != nil { // wrong number of columns
			fmt.Printf("Line: %sError: %s\n", line, err)
			continue
		}

		writerChannel <- record
	}
}

func processLine(headers []string, dataList []string) (map[string]string, error) {
	if len(dataList) != len(headers) {
		return nil, errors.New("Line does not match headers format. Skipping")
	}

	recordMap := make(map[string]string)

	for i, name := range headers {
		recordMap[name] = dataList[i]
	}

	return recordMap, nil
}

func check(e error) {
	if e != nil {
		exitGracefully(e)
	}
}

func exitGracefully(e error) {
	fmt.Fprintf(os.Stderr, "error: %v\n", e)
	os.Exit(1)
}

func main() {

}
