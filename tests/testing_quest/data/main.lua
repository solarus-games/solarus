function assert_equal(actual, expected)

  if actual ~= expected then
    error("equality assertion failed: expected '" ..
        tostring(expected) .. "', got '" ..
        tostring(actual) .. "'")
  end

end
