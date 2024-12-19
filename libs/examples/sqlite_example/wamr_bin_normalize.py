def replaceWordsInFile(fileName, before, after):

    with open(fileName, encoding="utf-8") as f:
        data_lines = f.read()

    # 指定したファイルの文字列を置換する
    data_lines = data_lines.replace(before, after)

    # 置換後に同じファイル名で保存する
    with open(fileName, mode="w", encoding="utf-8") as f:
        f.write(data_lines)

replaceWordsInFile("wamr_opcode.log", "0xc7", "0x20")
replaceWordsInFile("wamr_opcode.log", "0xc9", "0x21")
replaceWordsInFile("wamr_opcode.log", "0xca", "0x22")
